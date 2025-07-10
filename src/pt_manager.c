#include "../include/memory.h"
#include "../include/process.h"
#include <stdlib.h>
#include <assert.h>

int allocate_page_table(int slots_needed, PTB* blocks, int* pt_block_count, int pid) {

    print(blocks, pt_block_count);
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
            return merge_free_blocks(blocks, pt_block_count);
            
        }
    }
    return -1;
}

int compare_by_start(const void* a, const void* b) {
    PTB* pa = (PTB*)a;
    PTB* pb = (PTB*)b;
    return pa->start - pb->start;
}

int merge_free_blocks(PTB* blocks, int* pt_block_count) {
    // Ordina i blocchi per indirizzo di partenza
    qsort(blocks, *pt_block_count, sizeof(PTB), compare_by_start);

    int merge_cnt = 0;

    for (int i = 0; i < *pt_block_count - 1; ++i) {
        if (!blocks[i].used && !blocks[i + 1].used &&
            blocks[i].start + blocks[i].size == blocks[i + 1].start) {
            // merge i+1 into i
            blocks[i].size += blocks[i + 1].size;

            // shift tutti i blocchi successivi
            for (int k = i + 1; k < *pt_block_count - 1; ++k)
                blocks[k] = blocks[k + 1];
            (*pt_block_count)--;
            i--; // ricontrolla questo indice dopo merge

            merge_cnt++;
        }
    }

    return merge_cnt ? merge_cnt : -1;
}

void swap_block(PTB block){

    // printf("swap_block_debug1\n");
    process* p = &(process_list[block.pid]);
    // printf("swap_block_debug2\n");
    MMU* mmu =  process_data_list[block.pid].mmu;
    // printf("swap_block_debug3\n");

    p->pt_disk_start = mmu->disk_pt_index;
    // printf("swap_block_debug4\n");
    p->frames_disk_start = mmu->disk_frames_index;
    // printf("swap_block_debug2\n");
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
    // printf("swap_block_debug3\n");
    // update disk page table index
    mmu->disk_pt_index += p->pt_size;

    // copy frames on disk
    // printf("swap_block_debug4\n");
    for(int i = 0; i < p->pt_size; i++){
        // printf("swap_block_debug5\n");
        // free frame
        
        int frame_index = mmu->num_frames + (p->pt_ptr[i] * mmu->frame_size); // TODO: da capire
        for(int j = 0; j < mmu->frame_size; j++){
            // printf("swapping pysichal address in disk\n");
            mmu->DISK[mmu->disk_frames_index + j] = mmu->RAM[frame_index++];
        }
        // printf("swap_block_debug7\n");
        // after one frame is copied, update disk frame index 
        mmu->disk_frames_index += mmu->frame_size;
        // printf("swap_block_debug8\n");
    }

    p->on_disk = 1;

    return;

}

// this function is only called where allocate_page_table went wrong, so there are no free blocks to allocate a page table
// to do so we need to free some space in page table space and  allocate. In order to do so we swap the page table (and the frames) to DISK
int find_best_fit_to_evict(int slots_needed, PTB* blocks, int* pt_block_count, int* pt_blocks_index, int pt_space_size){

    printf("\npt_blocks_index : %d\n", *pt_blocks_index);
    printf("pt_space_size : %d\n", pt_space_size);
    printf("equation : %d\n", pt_space_size - *pt_blocks_index < slots_needed);
    
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
            printf("\npt_blocks_index mid : %d\n", *pt_blocks_index);
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
        printf("liberando il blocco in posizione %d -- pid %d\n\n", block_swap_idx, blocks[block_swap_idx].pid);
        swap_block(blocks[block_swap_idx]);
        int pid = blocks[block_swap_idx].pid;
        int merge_cnt = free_page_table(blocks[block_swap_idx].start, blocks, pt_block_count);

        // we merged from the front
        if (blocks[block_swap_idx].pid == pid){
            block_swap_idx++;
        }

        // we merged from the back
        // else {}
        
        print(blocks, pt_block_count);  
        
    }

    *pt_blocks_index += slots_needed;

    printf("\npt_blocks_index exit : %d\n", *pt_blocks_index);

    return base;
}

void print(PTB* blocks, int* pt_block_count){

    qsort(blocks, *pt_block_count, sizeof(PTB), compare_by_start);

    printf("\n");
    for (int i = 0; i < *pt_block_count; i++){
        printf("block %d: start -> %d , size -> %d, used-> %d\n", blocks[i].pid, blocks[i].start, blocks[i].size, blocks[i].used);
    }
    printf("\n");
}