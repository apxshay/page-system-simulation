#include "../include/memory.h"
#include <stdlib.h>

void mmu_init(MMU* mmu, MMU_config* config){
    // initializing memories
    mmu->ram_size = config->ram_size;
    mmu->disk_size = config->disk_size;
    mmu->num_frames = config->num_frames;

    mmu->RAM = calloc((mmu->num_frames + mmu->ram_size), sizeof(int));
    mmu->DISK = calloc(mmu->disk_size, sizeof(int));
    mmu->frame_bitmap = calloc(mmu->num_frames, sizeof(int));
    
    // initializing pt controller 
    mmu->max_pt_blocks = config->max_pt_blocks;
    mmu->pt_blocks = malloc(mmu->max_pt_blocks*sizeof(PTB));
    // size equals mmu->num_frames because page table space is as big as the number of frames in RAM
    mmu->pt_blocks[0] = (PTB){.start = 0, .size = mmu->num_frames, .used = 0};
    mmu->pt_block_count = 1;
}

// void page_table_print(){
//     printf("\n\n--- PAGE TABLE ---\n");
//     for(int i = 0; i < NUM_FRAMES_PER_PROCESS; i++){
//         printf("pt entry: %d    physichal frame: %d\n", i, page_table_ptr[i]);
//     }
//     printf("\n\n");
// }

// void page_assign(int pid){

//     int frames_assigned = NUM_FRAMES_PER_PROCESS;
//     for(int i = 0; i < NUM_FRAMES; i++){
//         //se ho assegnato tutti i frame allora esco dal ciclo
//         if (frames_assigned == 0) break;

//         //se il frame e libero allora lo assegno
//         if (frame_bitmap[i] == 0){
//             page_table_ptr[frames_assigned-1] = i;
//             frame_bitmap[i] = 1;
//             frames_assigned--;
            
//         }
//     }
// }