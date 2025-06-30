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
    config.tlb_replacement_policy = TLB_REPL_POLICY_FIFO;

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

        char filename[128];
        int tlb_miss_state = tlb_miss_count;
        int tlb_hit_state = tlb_hit_count;
        
        // Metti il path relativo della cartella test/
        snprintf(filename, sizeof(filename), "test1/process_%d.txt", i);
        FILE* fp = fopen(filename, "w");
        fprintf(fp, "---------------------\n");
        fprintf(fp, "|   vpn   |   pfn   |\n");
        fprintf(fp, "---------------------\n");

        for(int j = 0; j < p[i]->pt_size; j++){
            fprintf(fp, "|  %4d   |  %4d   |\n", j, p[i]->pt_ptr[j]);
        }
        fprintf(fp, "---------------------\n\n\nPROCESS ADRESSES GENERATION AND TRANSLATION\n\n");
        
        uint32_t vaddr = generate_vaddr(p[i], &p_data);

        for(int j = 0; j < 1000; j++){
            uint32_t paddr = translate_vaddr(p[i], &p_data, vaddr);
            fprintf(fp, "[TRANSLATE] vaddr=%u vpn=%#u pfn=%u offset=%u paddr=%u\n",
                vaddr, (vaddr & mmu.TLB->vpn_mask) >> mmu.TLB->offset_bits, p[i]->pt_ptr[(vaddr & mmu.TLB->vpn_mask) >> mmu.TLB->offset_bits],
       vaddr & mmu.TLB->offset_mask,
       paddr);

            vaddr = generate_vaddr_locality(vaddr, 90, p[i], &p_data);
        }
        fprintf(fp, "\n\nTLB HIT COUNT = %d\nTLB MISS COUNT = %d\nTLB HIT RATIO = %0.2f\nTLB MISS RATIO = %0.2f\n",
            tlb_hit_count - tlb_hit_state, 
            tlb_miss_count - tlb_miss_state,
            (double)(tlb_hit_count - tlb_hit_state)/(tlb_hit_count - tlb_hit_state + tlb_miss_count - tlb_miss_state),
            (double)(tlb_miss_count - tlb_miss_state)/(tlb_hit_count - tlb_hit_state + tlb_miss_count - tlb_miss_state)
        );
        
    }
    



}