#include "../include/memory.h"
#include "../include/process.h"
#include <stdlib.h>
#include <assert.h>

// aux function used to qsort an array made of PTB
int compare_by_start(const void* a, const void* b) {
    PTB* pa = (PTB*)a;
    PTB* pb = (PTB*)b;
    return pa->start - pb->start;
}


int allocate_page_table(int slots_needed, PTB* blocks, int* pt_block_count, int pid) {

    for (int i = 0; i < *pt_block_count; ++i) {

        if (!blocks[i].used && (blocks[i].size >= slots_needed)) {
            
            int base = blocks[i].start;
            // mark as used if block size is exactly as requested
            if (blocks[i].size == slots_needed) {
                blocks[i].used = 1;
                blocks[i].pid = pid;
            } else {
    
                // else, cut the block
                blocks[i].start += slots_needed;
                blocks[i].size -= slots_needed;

                // add new used block
                blocks[*pt_block_count] = (PTB){.start = base, .size = slots_needed, .used = 1, .pid = pid};
                (*pt_block_count)++;
            }
            
            return base;
        }
    }
    
    return -1;  // no block big enough
}



// to free a blocks its not used the block index, because it may change upon sorting the blocks array
// it uses instead the ram index (base) where the page table starts 
int  free_page_table(int base, PTB* blocks, int* pt_block_count) {
    for (int i = 0; i < *pt_block_count; ++i) {
        if (blocks[i].start == base && blocks[i].used) {
            blocks[i].used = 0;
            blocks[i].pid = -1;
            return merge_free_blocks(blocks, pt_block_count);
            
        }
    }
    return -1;
}



int merge_free_blocks(PTB* blocks, int* pt_block_count) {
    
    qsort(blocks, *pt_block_count, sizeof(PTB), compare_by_start);

    int merge_cnt = 0;

    for (int i = 0; i < *pt_block_count - 1; ++i) {
        if (!blocks[i].used && !blocks[i + 1].used &&
            blocks[i].start + blocks[i].size == blocks[i + 1].start) {
            // merge i+1 into i
            blocks[i].size += blocks[i + 1].size;

            // shift next blocks
            for (int k = i + 1; k < *pt_block_count - 1; ++k)
                blocks[k] = blocks[k + 1];
            (*pt_block_count)--;
            i--; 

            merge_cnt++;
        }
    }

    return merge_cnt ? merge_cnt : -1;
}

void swap_block(PTB block){
    
    if (block.used == 0) return;

    process* p = &(process_list[block.pid]);    
    MMU* mmu =  process_data_list[block.pid].mmu;

    p->pt_disk_start = mmu->disk_pt_index;
    p->frames_disk_start = mmu->disk_frames_index;

    // copy page table on disk
    for(int i = 0; i < block.size; i++){
        
        for(int j = 0; j < mmu->num_frames; j++){
            if (mmu->frames[j].idx == p->pt_ptr[i]) {
                mmu->frames[j].used = 0;
            }
        }
    }

    for(int i = 0; i < p->pt_size; i++){
        mmu->DISK[mmu->disk_pt_index + i] = p->pt_ptr[i];
    }

    // update disk page table index
    mmu->disk_pt_index += p->pt_size;

    // copy frames on disk
    for(int i = 0; i < p->pt_size; i++){
        
        // free frame
        int frame_index = mmu->num_frames + (p->pt_ptr[i] * mmu->frame_size); // TODO: to handle
        for(int j = 0; j < mmu->frame_size; j++){
        
            mmu->DISK[mmu->disk_frames_index + j] = mmu->RAM[frame_index++];
        }
        
        // after one frame is copied, update disk frame index 
        mmu->disk_frames_index += mmu->frame_size;
        
    }

    FILE * process_log = fopen("output/process_creation_log.txt", "a");
    fprintf(process_log, "[SWAP OUT] Swapping out process with PID: %d\n\n", p->pid);fflush(process_log);
    printf("SWAPPING OUT PROCESS WITH PID %d\n", p->pid);
    swap_out_count++;

    p->on_disk = 1;

    return;

}

// this function is only called where allocate_page_table went wrong, so there are no free blocks to allocate a page table
// to do so we need to free some space in page table space and  allocate. In order to do so we swap the page table (and the frames) to DISK
int find_best_fit_to_evict(int slots_needed, PTB* blocks, int* pt_block_count, int* pt_blocks_index, int pt_space_size){

    
    if (pt_space_size - *pt_blocks_index < slots_needed){
        *pt_blocks_index = 0;
    }
    
    qsort(blocks, *pt_block_count, sizeof(PTB), compare_by_start);
    
    int first_block_to_swap = -1;
    int base = -1;


    for(int i = 0; i < *pt_block_count; i++){
        if (blocks[i].start + blocks[i].size > *pt_blocks_index){
            first_block_to_swap = i;
            *pt_blocks_index = blocks[i].start;
            base = blocks[i].start;
            break;
        }
    }

    assert(first_block_to_swap != -1);

    // find blocks to swap and return them, or swap them directly and return the base in order to allocate from there
    int block_swap_idx = first_block_to_swap;
    int freed_slots = slots_needed;
    while(block_swap_idx < *pt_block_count && freed_slots > 0){
        freed_slots -= blocks[block_swap_idx].size;     

        int check = 0;
        
        // here there are the cases were we need to increment the block_swap_idx, in all the other cases, the blocks merged so that the next block to swap is already in position
        if (
            (block_swap_idx+1 < *pt_block_count && blocks[block_swap_idx+1].pid == -1 && !(block_swap_idx>=1 && blocks[block_swap_idx-1].pid == -1)) ||
            (block_swap_idx >= 1 && block_swap_idx+1 < *pt_block_count && blocks[block_swap_idx-1].pid != -1 && blocks[block_swap_idx+1].pid != -1) ||
            (block_swap_idx == 0 && blocks[block_swap_idx+1].pid != -1)
        ){
            check = 1;
        }

        swap_block(blocks[block_swap_idx]);
        free_page_table(blocks[block_swap_idx].start, blocks, pt_block_count);

        // we increment only if check is set to 1
        if (check == 1) block_swap_idx++;

        
    }

    *pt_blocks_index += slots_needed;

    return base;
}

