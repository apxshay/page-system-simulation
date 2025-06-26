#include <stdio.h>
#include <stdlib.h>

#include "include/process.h"
#include "include/memory.h"

MMU mmu;
MMU_config config;

int main(int argv, char** argc){

    config.disk_size = DISK_SIZE;
    config.ram_size = RAM_SIZE;
    config.max_pt_blocks = RAM_SIZE / 2;
    config.num_frames = NUM_FRAMES;

    mmu_init(&mmu, &config);
    
    print(mmu.pt_blocks, &mmu.pt_block_count);
    allocate_page_table(12, mmu.pt_blocks, &mmu.pt_block_count);
    print(mmu.pt_blocks, &mmu.pt_block_count);
    allocate_page_table(18, mmu.pt_blocks, &mmu.pt_block_count);
    print(mmu.pt_blocks, &mmu.pt_block_count);
    allocate_page_table(4, mmu.pt_blocks, &mmu.pt_block_count);
    print(mmu.pt_blocks, &mmu.pt_block_count);
    allocate_page_table(12, mmu.pt_blocks, &mmu.pt_block_count);
    print(mmu.pt_blocks, &mmu.pt_block_count);
    allocate_page_table(17, mmu.pt_blocks, &mmu.pt_block_count);
    print(mmu.pt_blocks, &mmu.pt_block_count);
    free_page_table(30, mmu.pt_blocks, &mmu.pt_block_count);
    free_page_table(34, mmu.pt_blocks, &mmu.pt_block_count);
    print(mmu.pt_blocks, &mmu.pt_block_count);
    allocate_page_table(16, mmu.pt_blocks, &mmu.pt_block_count);
    print(mmu.pt_blocks, &mmu.pt_block_count);
    allocate_page_table(12, mmu.pt_blocks, &mmu.pt_block_count);


}