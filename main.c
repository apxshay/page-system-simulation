#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "include/process.h"
#include "include/memory.h"

MMU mmu;
MMU_config config;

int main(int argv, char** argc){

    srand(time(NULL));

    config.disk_size = DISK_SIZE;
    config.ram_size = ACTUAL_RAM_SIZE;
    config.max_pt_blocks = RAM_SIZE / 2;
    config.num_frames = NUM_FRAMES;
    config.tlb_size = TLB_SIZE;
    config.tlb_replacement_policy = TLB_REPL_POLICY_CLOCK;

    mmu_init(&mmu, &config);
    process* p[10];
    
    for (int i = 0; i < 10; i++){

        //init tlb
        for (int j = 0; j < mmu.TLB->tlb_size; j++){
            mmu.TLB->entries[j].valid = 0;
            mmu.TLB->entries[j].referenced = 0;
        }

        int frames = 1 + rand() % 20;
        process_data p_data = {
            .mmu = &mmu, 
            .frames_requested = frames
        };
        p[i] = create_process(NULL, &p_data);;
        if (p[i] == NULL){
            printf("impossibile to create process\n");
            continue;
        }

        for(int j = 0; j < p[i]->pt_size; j++){
            printf("pte: %d  frame:  %d\n", j, p[i]->pt_ptr[j]);
        }
        uint32_t vaddr = generate_vaddr(p[i], &p_data);
        for(int j = 0; j < 100; j++){
            uint32_t paddr = translate_vaddr(p[i], &p_data, vaddr);
            printf("[TRANSLATE] vaddr=%#06x vpn=%#06x pfn=%#06x offset=%#06x paddr=%#06x\n",
       vaddr, (vaddr & mmu.TLB->vpn_mask) >> mmu.TLB->offset_bits, p[i]->pt_ptr[(vaddr & mmu.TLB->vpn_mask) >> mmu.TLB->offset_bits],
       vaddr & mmu.TLB->offset_mask,
       paddr);

            vaddr = generate_vaddr_locality(vaddr, 90, p[i], &p_data);
        }
        
    }
    printf("\n\nTLB HIT COUNT = %d\nTLB MISS COUNT = %d\nTLB HIT RATIO = %f\nTLB MISS RATIO = %f\n",
        tlb_hit_count, 
        tlb_miss_count,
        (double)tlb_hit_count/(tlb_hit_count + tlb_miss_count),
        (double)tlb_miss_count/(tlb_hit_count + tlb_miss_count)
    );



}