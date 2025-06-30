#ifndef TLB_H
#define TLB_H

#include <stdint.h>

typedef struct TLB TLB;

typedef void (*TLBUpdateFunc)(TLB* tlb, uint32_t vpn, uint32_t pfn);

typedef struct TLBEntry{
    uint32_t vpn;
    uint32_t pfn;
    int valid;
    int referenced;
} TLBEntry;

typedef enum {
    TLB_REPL_POLICY_FIFO,
    TLB_REPL_POLICY_CLOCK
    //TLB_REPL_POLICY_LRU,
    //TLB_REPL_POLICY_RANDOM
} TLBReplacementPolicy;

typedef struct TLB{
    TLBEntry* entries;
    int tlb_size;
    int vpn_bits;
    int offset_bits;
    uint32_t vpn_mask;
    uint32_t offset_mask;

    TLBReplacementPolicy replacement_policy;
    
    int replace_index;
    TLBUpdateFunc update_policy;

} TLB;



void tlb_update_fifo(TLB* tlb, uint32_t vpn, uint32_t pfn);
void tlb_update_clock(TLB* tlb, uint32_t vpn, uint32_t pfn);


#endif