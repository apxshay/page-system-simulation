#include "../include/memory.h"

int allocate_page_table(int slots_needed, PTB* blocks, int* pt_block_count) {
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
            return base;
        }
    }
    return -1;  // no block big enough
}

void free_page_table(int base, PTB* blocks, int* pt_block_count) {
    for (int i = 0; i < *pt_block_count; ++i) {
        if (blocks[i].start == base && blocks[i].used) {
            blocks[i].used = 0;
            merge_free_blocks(blocks, pt_block_count);
            return;
        }
    }
}

void merge_free_blocks(PTB* blocks, int* pt_block_count) {
    for (int i = 0; i < *pt_block_count; ++i) {
        if (!blocks[i].used) {
            for (int j = i + 1; j < *pt_block_count; ++j) {
                if (!blocks[j].used &&
                    blocks[i].start + blocks[i].size == blocks[j].start) {
                    // merge j in i
                    blocks[i].size += blocks[j].size;

                    // remove block j
                    for (int k = j; k < *pt_block_count - 1; ++k)
                        blocks[k] = blocks[k + 1];
                    (*pt_block_count)--;
                    j--;
                }
            }
        }
    }
}

void print(PTB* blocks, int* pt_block_count){
    printf("\n");
    for (int i = 0; i < *pt_block_count; i++){
        printf("block %d: start -> %d , size -> %d, used-> %d\n", i, blocks[i].start, blocks[i].size, blocks[i].used);
    }
    printf("\n");
}