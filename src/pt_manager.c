#include "../include/memory.h"
#include <stdlib.h>
#include <assert.h>

int allocate_page_table(int slots_needed, PTB* blocks, int* pt_block_count, int pid) {
    for (int i = 0; i < *pt_block_count; ++i) {
        if (!blocks[i].used && blocks[i].size >= slots_needed) {
            int base = blocks[i].start;
            // mark as used if block size is exactly as requested
            if (blocks[i].size == slots_needed) {
                blocks[i].used = 1;
            } else {
                // else, cut the block
                blocks[i].start += slots_needed;
                blocks[i].size -= slots_needed;

                // add new used block
                blocks[*pt_block_count] = (PTB){.start = base, .size = slots_needed, .used = 1};
                (*pt_block_count)++;
            }
            blocks[i].pid = pid;
            return base;
        }
    }
    return -1;  // no block big enough
}



// to free a blocks its not used the block index, because it may change upon sorting the blocks array
// it uses instead the ram index (base) where the page table starts 
void free_page_table(int base, PTB* blocks, int* pt_block_count) {
    for (int i = 0; i < *pt_block_count; ++i) {
        if (blocks[i].start == base && blocks[i].used) {
            blocks[i].used = 0;
            merge_free_blocks(blocks, pt_block_count);
            return;
        }
    }
}

int compare_by_start(const void* a, const void* b) {
    PTB* pa = (PTB*)a;
    PTB* pb = (PTB*)b;
    return pa->start - pb->start;
}

void merge_free_blocks(PTB* blocks, int* pt_block_count) {
    // Ordina i blocchi per indirizzo di partenza
    qsort(blocks, *pt_block_count, sizeof(PTB), compare_by_start);

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
        }
    }
}

// this function is only called where allocate_page_table whent wrong, so there are no free blocks to allocate a page table
// to do so we need to free some space in page table space and  allocate. In order to do so we swap the page table (and the frames) to DISK
int find_best_fit_to_evict(int slots_needed, PTB* blocks, int* pt_block_count, int pt_blocks_index, int pt_space_size){
    
    if (pt_space_size - pt_blocks_index < slots_needed){
        pt_blocks_index = 0;
    }
    
    qsort(blocks, *pt_block_count, sizeof(PTB), compare_by_start);

    int first_block_to_swap = -1;
    int base = -1;


    for(int i = 0; i < *pt_block_count; i++){
        if (blocks[i].start + blocks[i].size >= pt_blocks_index){
            first_block_to_swap = i;
            pt_blocks_index = blocks[i].start;
            base = blocks[i].start;
            break;
        }
    }
    
    assert(first_block_to_swap != -1);

    // TODO: find blocks to swap and return them, or swap them directly and return the base in order to allocate from there
    int block_swap_idx = first_block_to_swap;
    int freed_slots = slots_needed;
    
    while(block_swap_idx < *pt_block_count && freed_slots > 0){
        freed_slots -= blocks[block_swap_idx].size;
        swap_block(blocks[block_swap_idx++]);
    }


    return base;
}

void print(PTB* blocks, int* pt_block_count){
    printf("\n");
    for (int i = 0; i < *pt_block_count; i++){
        printf("block %d: start -> %d , size -> %d, used-> %d\n", i, blocks[i].start, blocks[i].size, blocks[i].used);
    }
    printf("\n");
}