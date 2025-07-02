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
                            &(config->mmu->pt_block_count),
                            p->pid);
    if (pt_base < 0){
        // TODO: find best fit, replace it and allocate new page table and map frames
        int pt_to_evict_base = find_best_fit_to_evict(p->pt_size, 
                                                      config->mmu->pt_blocks,
                                                    &(config->mmu->pt_block_count));
        if (pt_to_evict_base < 0){
            // no block is big enough to be replaced and respect the answer, so the process can not be created
            return NULL;
        }
        
    } 

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
    uint32_t vpn = (p->pt_size - 1) << config->mmu->TLB->offset_bits;
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

    uint32_t vpn = (rand() % p->pt_size) << data->mmu->TLB->offset_bits;
    uint32_t offset = (rand() % data->mmu->frame_size);

    uint32_t vaddr = vpn | offset;
    return vaddr;
    
}

uint32_t generate_vaddr_locality(uint32_t base_vaddr, int locality_percent, process* p, process_data* data) {
    int r = 1 + rand() % 100;

    if (r <= locality_percent){
        // generate with locality
        int64_t offset = (rand() % ((2*FRAME_SIZE) + 1)) - FRAME_SIZE; // in range [addr-64, addr+64]
        int64_t new_addr = (int64_t)base_vaddr + offset;
        if (new_addr < 0) new_addr = 0;
        if (new_addr >= p->max_vaddr) new_addr = p->max_vaddr - 1;
        return (uint32_t)new_addr;
    }
    
    // generate random
    return generate_vaddr(p, data);
}

uint32_t tlb_lookup(TLB* tlb, uint32_t vaddr){
    uint32_t vpn = (vaddr & tlb->vpn_mask) >> tlb->offset_bits;
    for (int i = 0; i < tlb->tlb_size; i++){
        if (tlb->entries[i].valid == 1 && tlb->entries[i].vpn == vpn){
            return tlb->entries[i].pfn;
        }
    }
    return TLB_MISS;
}

void update_tlb(TLB* tlb, uint32_t vpn, uint32_t pfn){
    for(int i = 0; i < tlb->tlb_size; i++){
        if (tlb->entries[i].valid == 0){
            tlb->entries[i].vpn = vpn;
            tlb->entries[i].valid = 1;
            tlb->entries[i].referenced = 1;
            tlb->entries[i].pfn = pfn;
            return;
        }
    }

    tlb->update_policy(tlb, vpn, pfn);
}

uint32_t translate_vaddr(process* p, process_data* data, uint32_t vaddr){
    
    uint32_t pfn = tlb_lookup(data->mmu->TLB, vaddr);
    if (pfn != TLB_MISS){ // TLB HIT
        tlb_hit_count++;
        uint32_t paddr = (pfn << data->mmu->TLB->offset_bits) | (vaddr & data->mmu->TLB->offset_mask);
        return paddr;
    }
    else { // TLB MISS
        tlb_miss_count++;
        uint32_t vpn = (vaddr & data->mmu->TLB->vpn_mask) >> data->mmu->TLB->offset_bits;
        pfn = p->pt_ptr[vpn];

        update_tlb(data->mmu->TLB, vpn, pfn);

        uint32_t paddr =  (pfn << data->mmu->TLB->offset_bits) | (vaddr & data->mmu->TLB->offset_mask);
        return paddr;
    }
}