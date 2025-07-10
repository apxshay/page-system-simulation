#include "../include/memory.h"
#include <stdlib.h>
#include <time.h>


// AUX FUNCTIONS

static void shuffle_frames(Frame* frames, int n) {

    static int initialized = 0;
    if (!initialized) {
        srand(time(NULL));
        initialized = 1;
    }

    for (int i = n - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        Frame temp = frames[i];
        frames[i] = frames[j];
        frames[j] = temp;
    }
}

static int log2(int n) {

    int res = 0;
    while (n >>= 1) ++res;
    return res;
}


// AUX FUNCTIONS END



void mmu_init(MMU* mmu, MMU_config* config){
    // initializing memories
    mmu->ram_size = config->ram_size + config->num_frames; // e.g. actual ram is 4096 with 64 frames, then we add 64 bytes to store 64 bytes max page table
    mmu->disk_size = config->disk_size;
    mmu->num_frames = config->num_frames;
    mmu->frame_size = (mmu->ram_size - mmu->num_frames) / mmu->num_frames; // dont count page table space, so subtract number of frames 

    mmu->RAM = calloc((mmu->num_frames + mmu->ram_size), sizeof(int));
    mmu->DISK = calloc(mmu->disk_size, sizeof(int));

    // initializing frames
    mmu->frames = malloc(mmu->num_frames*sizeof(Frame));
    for(int i = 0; i < mmu->num_frames; i++){
        mmu->frames[i].idx = i;
        mmu->frames[i].used = 0;
    }

    shuffle_frames(mmu->frames, mmu->num_frames);

    // initializing pt controller 
    mmu->max_pt_blocks = config->max_pt_blocks;
    mmu->pt_blocks = malloc(mmu->max_pt_blocks*sizeof(PTB));
    // size equals mmu->num_frames because page table space is as big as the number of frames in RAM
    mmu->pt_blocks[0] = (PTB){.start = 0, .size = mmu->num_frames, .used = 0, .pid = -1};
    mmu->pt_block_count = 1;

    // initializing virtual address stuff
    mmu->TLB = malloc(sizeof(TLB));
    mmu->TLB->replace_index = 0;
    mmu->TLB->tlb_size = config->tlb_size;
    mmu->TLB->entries = malloc(mmu->TLB->tlb_size * sizeof(TLBEntry));
    // initializing each entry
    for (int i = 0; i < mmu->TLB->tlb_size; i++){
        mmu->TLB->entries[i].pfn = 0;
        mmu->TLB->entries[i].vpn = 0;
        mmu->TLB->entries[i].referenced = 0;
        mmu->TLB->entries[i].valid = 0;
    }

    mmu->TLB->vpn_bits = log2(mmu->num_frames);
    mmu->TLB->offset_bits = log2(mmu->frame_size);

    mmu->TLB->vpn_mask = ((1U << mmu->TLB->vpn_bits) - 1) << mmu->TLB->offset_bits;
    mmu->TLB->offset_mask = (1U << mmu->TLB->offset_bits) - 1;

    // managing tlb replacement policy and ops 
    mmu->TLB->replacement_policy = config->tlb_replacement_policy;
    switch (mmu->TLB->replacement_policy)
    {
    case TLB_REPL_POLICY_FIFO:
        mmu->TLB->update_policy = tlb_update_fifo;
        break;
    case TLB_REPL_POLICY_CLOCK:
        mmu->TLB->update_policy = tlb_update_clock;
        break;
    default:
        break;
    }

    // virtual memory
    mmu->disk_pt_index = 0;
    mmu->disk_frames_index = config->ram_size; // TODO: da capire
}

int find_free_frame(MMU* mmu){

    for(int i = 0; i < mmu->num_frames; i++){
        if (mmu->frames[i].used == 0){
            mmu->frames[i].used = 1;
            return mmu->frames[i].idx;
        }
    }

    return -1;
}

