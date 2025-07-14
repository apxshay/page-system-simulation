#ifndef MEMORY_H
#define MEMORY_H

#include "global.h"
#include <stdio.h>
#include "tlb.h"

#define NO_FIT 10000

typedef struct PageTableBlock PTB;

typedef struct PageTableBlock{
    int start;
    int size;
    int used;

    int pid;
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
    int pt_blocks_index; // index where from page table space i need to start the eviction
    int max_pt_blocks;
    int pt_block_count;

    int ram_size;
    int disk_size;
    int num_frames;
    int frame_size;

    // virtual addresses and TLB
    TLB* TLB;

    // information for virtual memory
    int disk_pt_index;
    int disk_frames_index;
} MMU;

typedef struct MMU_config{
    int ram_size;
    int disk_size;
    int tlb_size;
    int num_frames;
    int max_pt_blocks;
    TLBReplacementPolicy tlb_replacement_policy;
} MMU_config;


void mmu_init(MMU* mmu, MMU_config* config);

// page table manager funtions
int allocate_page_table(int slots_needed, PTB* blocks, int* pt_block_count, int pid);
int free_page_table(int base, PTB* blocks, int* pt_block_count);
int merge_free_blocks(PTB* blocks, int* pt_block_count);

int find_free_frame(MMU* mmu);

int find_best_fit_to_evict(int slots_needed, PTB* blocks, int* pt_block_count, int* pt_blocks_index, int pt_space_size);

extern void print_ptb(PTB* blocks, int* pt_block_count);

extern void print_ptb_to_file(PTB* blocks, int* pt_block_count, const char* filename);

int compare_by_start(const void* a, const void* b);


#endif