#include "../include/process.h"
#include <stdlib.h>


process* create_process(process* p, process_data* config){
    if (!(p = malloc(sizeof(process)))){
        return NULL;
    }
    
    p->pid = active_process_cnt++;
    p->pt_size = config->frames_requested;

    int pt_base = allocate_page_table(p->pt_size, 
                            config->mmu->pt_blocks, 
                            &(config->mmu->pt_block_count));
    if (pt_base < 0) return NULL;

    p->pt_ptr = config->mmu->RAM + pt_base;
    
    return p;
}



void process_destroy(process* p, process_data* data){
    free_page_table(p->pt_ptr - data->mmu->RAM, 
                    data->mmu->pt_blocks, 
                    &(data->mmu->pt_block_count));
    free(p);
}