#ifndef MEMORY_H
#define MEMORY_H

#include "global.h"
#include <stdio.h>

typedef struct PageTableBlock{
    int start;
    int size;
    int used;

} PTB;

typedef struct FrameInfo{
    int idx;
    int used;
} Frame;

typedef struct MMU{
    int* RAM;
    int* DISK;
    Frame* frames;

    // information needed for page tables
    PTB* pt_blocks;
    int max_pt_blocks;
    int pt_block_count;

    int ram_size;
    int disk_size;
    int num_frames;
    int frame_size;
    int vpn_bits;
    int offset_bits;

    uint32_t vpn_mask;
    uint32_t offset_mask;

    
} MMU;

typedef struct MMU_config{
    int ram_size;
    int disk_size;
    int num_frames;
    int max_pt_blocks;
} MMU_config;


void mmu_init(MMU* mmu, MMU_config* config);

// page table manager funtions
int allocate_page_table(int slots_needed, PTB* blocks, int* pt_block_count);
void free_page_table(int base, PTB* blocks, int* pt_block_count);
void merge_free_blocks(PTB* blocks, int* pt_block_count);
void print(PTB* blocks, int* pt_block_count);

// mapping frames to page table
int find_free_frame(MMU* mmu);




#endif