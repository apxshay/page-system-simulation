#include "../include/tlb.h"

void tlb_update_fifo(TLB* tlb, uint32_t vpn, uint32_t pfn){
    tlb->entries[tlb->replace_index].vpn = vpn;
    tlb->entries[tlb->replace_index].pfn = pfn;
    tlb->entries[tlb->replace_index].valid = 1;

    tlb->replace_index = (tlb->replace_index + 1) % tlb->tlb_size;
    
}


void tlb_update_clock(TLB* tlb, uint32_t vpn, uint32_t pfn){
    while(tlb->entries[tlb->replace_index].referenced == 1){
        tlb->entries[tlb->replace_index].referenced = 0;
        tlb->replace_index = (tlb->replace_index + 1) % tlb->tlb_size;
    }
    
    // replace
    tlb->entries[tlb->replace_index].vpn = vpn;
    tlb->entries[tlb->replace_index].pfn = pfn;
    tlb->entries[tlb->replace_index].valid = 1;
    tlb->entries[tlb->replace_index].referenced = 1;

    tlb->replace_index = (tlb->replace_index + 1) % tlb->tlb_size;
    
}