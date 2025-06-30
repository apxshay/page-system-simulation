#include "../include/process.h"
#include <stdlib.h>
#include <math.h>


process* create_process(process* p, process_data* config){
    if (!(p = malloc(sizeof(process)))){
        return NULL;
    }
    
    
    p->pt_size = config->frames_requested;

    // allocate page table in RAM
    int pt_base = allocate_page_table(p->pt_size, 
                            config->mmu->pt_blocks, 
                            &(config->mmu->pt_block_count));
    if (pt_base < 0) return NULL;

    p->pt_ptr = config->mmu->RAM + pt_base;

    // map frames into page table
    // printf("n_frames: %d\n", p->pt_size);
    for(int i = 0; i < p->pt_size; i++){
        int free_frame = find_free_frame(config->mmu);
        
        if (free_frame < 0){
            return NULL;
        }
        p->pt_ptr[i] = free_frame;
    }

    
    p->pid = active_process_cnt++;
    

    // generating max virtual address
    uint32_t vpn = (p->pt_size - 1) << config->mmu->offset_bits;
    uint32_t offset = config->mmu->frame_size - 1;
    
    p->max_vaddr = vpn | offset;
    
    return p;
}



void process_destroy(process* p, process_data* data){

    // deallocate/free frames from page table 
    for(int i = 0; i < p->pt_size; i++){
        int frame_idx = p->pt_ptr[i];

        // look in the frames array for the correct element
        for(int j = 0; j < data->mmu->num_frames; j++){
            if (frame_idx == (data->mmu->frames)[j].idx){
                (data->mmu->frames)[j].used = 0;
            }
        }
    }

    free_page_table(p->pt_ptr - data->mmu->RAM, 
                    data->mmu->pt_blocks, 
                    &(data->mmu->pt_block_count));
    free(p);
    active_process_cnt--;
}

uint32_t generate_vaddr(process* p, process_data* data){

    uint32_t vpn = (rand() % p->pt_size) << data->mmu->offset_bits;
    uint32_t offset = (rand() % data->mmu->frame_size);

    uint32_t vaddr = vpn | offset;
    return vaddr;
    
}