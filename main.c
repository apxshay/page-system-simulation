#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "include/process.h"
#include "include/memory.h"

MMU mmu;
MMU_config config;

void print_process_list(process* list, int num_processes) {
    printf("=== Process List ===\n");
    for (int i = 0; i < num_processes; i++) {
        process p = list[i];
        printf("PID: %d\n", p.pid);
        printf("  Page Table Size: %d\n", p.pt_size);
        printf("  On Disk: %s\n", p.on_disk ? "Yes" : "No");
        printf("  PT Disk Start: %d\n", p.pt_disk_start);
        printf("  Frames Disk Start: %d\n", p.frames_disk_start);
        printf("  Max Virtual Address: %u\n", p.max_vaddr);

        printf("  Page Table [pt_ptr]: ");
        if (p.pt_ptr != NULL) {
            for (int j = 0; j < p.pt_size; j++) {
                printf("%d ", p.pt_ptr[j]);
            }
        } else {
            printf("NULL");
        }
        printf("\n");
    }
    printf("====================\n");
}



int main(int argv, char** argc){

    srand(time(NULL));

    config.disk_size = DISK_SIZE;
    config.ram_size = ACTUAL_RAM_SIZE;
    config.max_pt_blocks = RAM_SIZE / 2;
    config.num_frames = NUM_FRAMES;
    config.tlb_size = TLB_SIZE;
    config.tlb_replacement_policy = TLB_REPL_POLICY_FIFO;

    mmu_init(&mmu, &config);
    
    
    for (int i = 0; i < NUM_PROCESSES; i++){

        //init tlb
        for (int j = 0; j < mmu.TLB->tlb_size; j++){
            mmu.TLB->entries[j].valid = 0;
            mmu.TLB->entries[j].referenced = 0;
        }

        int frames = 1 + rand() % 20;
        process_data_list[i].mmu = &mmu; 
        process_data_list[i].frames_requested = frames;
        process* new_process = create_process(NULL, &(process_data_list[i]));
        if (new_process == NULL) {
            printf("Impossible to create process\n");
            continue;
        }
        process_list[i] = *new_process;
        print_process_list(process_list, i+1);

        char filename[128];
        int tlb_miss_state = tlb_miss_count;
        int tlb_hit_state = tlb_hit_count;
        
        // Metti il path relativo della cartella test/
        snprintf(filename, sizeof(filename), "test1/process_%d.txt", i);
        FILE* fp = fopen(filename, "w");
        fprintf(fp, "---------------------\n");
        fprintf(fp, "|   vpn   |   pfn   |\n");
        fprintf(fp, "---------------------\n");

        for(int j = 0; j < process_list[i].pt_size; j++){
            fprintf(fp, "|  %4d   |  %4d   |\n", j, process_list[i].pt_ptr[j]);
        }
        fprintf(fp, "---------------------\n\n\nPROCESS ADRESSES GENERATION AND TRANSLATION\n\n");
        
        uint32_t vaddr = generate_vaddr(&process_list[i], &(process_data_list[i]));

        for(int j = 0; j < 1000; j++){
            uint32_t paddr = translate_vaddr(&process_list[i], &(process_data_list[i]), vaddr);
            fprintf(fp, "[TRANSLATE] vaddr=%u vpn=%#u pfn=%u offset=%u paddr=%u\n",
                vaddr, (vaddr & mmu.TLB->vpn_mask) >> mmu.TLB->offset_bits, process_list[i].pt_ptr[(vaddr & mmu.TLB->vpn_mask) >> mmu.TLB->offset_bits],
       vaddr & mmu.TLB->offset_mask,
       paddr);

            vaddr = generate_vaddr_locality(vaddr, 90, &process_list[i], &(process_data_list[i]));
        }
        fprintf(fp, "\n\nTLB HIT COUNT = %d\nTLB MISS COUNT = %d\nTLB HIT RATIO = %0.2f\nTLB MISS RATIO = %0.2f\n",
            tlb_hit_count - tlb_hit_state, 
            tlb_miss_count - tlb_miss_state,
            (double)(tlb_hit_count - tlb_hit_state)/(tlb_hit_count - tlb_hit_state + tlb_miss_count - tlb_miss_state),
            (double)(tlb_miss_count - tlb_miss_state)/(tlb_hit_count - tlb_hit_state + tlb_miss_count - tlb_miss_state)
        );
        
    }
    



}

