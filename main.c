#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdint.h>
#include <string.h>

#include "include/process.h"
#include "include/memory.h"
#include "include/print_utils.h"

MMU mmu;
MMU_config config;

int main(int argv, char** argc){

    srand(time(NULL));

    int locality_of_reference = 0;
    int num_processes = 0;
    int num_addresses = 0;

    if (argv > 1 && strcmp(argc[1], "config") == 0){
        load_config_from_file("config.txt", &config, &locality_of_reference, &num_processes, &num_addresses);
    }
    else if (argv > 1 && strcmp(argc[1], "config") != 0){
        printf("\n\nRUN EXEC WITH \"config\" PARAM\n\n\n");
        return 0;
    }
    else {
        config.disk_size = DISK_SIZE;
        config.ram_size = ACTUAL_RAM_SIZE;
        config.max_pt_blocks = NUM_FRAMES;
        config.num_frames = NUM_FRAMES;
        config.tlb_size = TLB_SIZE;
        config.tlb_replacement_policy = TLB_REPL_POLICY_CLOCK;

        locality_of_reference = LOCALITY_PERC;
        num_processes = NUM_PROCESSES;
        num_addresses = NUM_ADDRESSES;
    
    }

    process_list = malloc(num_processes * sizeof(process));
    process_data_list = malloc(num_processes * sizeof(process_data));
    
    
    

    


    mmu_init(&mmu, &config);

    printf("\n\n======== SYSTEM CONFIGURATION ========\n\n");
    printf("RAM size: %d slots + %d slots ( page table space )\n", config.ram_size, config.num_frames);
    printf("DISK size: %d slots\n", config.disk_size);
    printf("TLB size: %d entries\n", config.tlb_size);
    printf("TLB replacement policy: %s\n", config.tlb_replacement_policy == TLB_REPL_POLICY_FIFO ? "FIFO" : "CLOCK");
    printf("Frames: %d frames\n", config.num_frames);
    printf("Frame size: %d addresses\n", config.ram_size / config.num_frames);
    printf("Processes: %d processes\n", num_processes);
    
    printf("Locality of reference: %d%%\n\n", locality_of_reference);
    printf("======== SYSTEM CONFIGURATION ========\n\n");

    printf("======== CREATING %d PROCESSES ========\n\n", num_processes);

    FILE* process_log = fopen("output/process_creation_log.txt", "a");
    
    
    for (int i = 0; i < num_processes; i++){

        //init tlb
        for (int j = 0; j < mmu.TLB->tlb_size; j++){
            mmu.TLB->entries[j].valid = 0;
            mmu.TLB->entries[j].referenced = 0;
        }

        //creating new process
        
        int frames = 1 + rand() % 20;

        fprintf(process_log, "\n\n\n\n\n\n\n========  CREATING NEW PROCESS WITH PID: %d  ========\n\n", active_process_cnt);fflush(process_log);

        fprintf(process_log, "PID: %d\n", active_process_cnt);
        fprintf(process_log, "FRAMES REQUESTED: %d frames\n", frames);
        fprintf(process_log, "\n");

        fflush(process_log);

        printf("--------------------------------\n");
        printf("CREATING A PROCESS WITH PID: %1d --- SIZE %1d FRAMES\n", active_process_cnt, frames);
        
        process_data_list[i].mmu = &mmu; 
        process_data_list[i].frames_requested = frames;
        process* new_process = create_process(NULL, &(process_data_list[i]));

        //checking wheter process creation went wrong
        if (new_process == NULL) {
            printf("Impossible to create process\n");
            continue;
        }
        process_list[i] = *new_process;

        printf("CORRECTLY CREATED PROCESS WITH PID: %d\n", new_process->pid);
        printf("--------------------------------\n\n");

        print_process_list_to_file(process_list, i+1, process_log);fflush(process_log);

        char filename[128];
        int tlb_miss_state = tlb_miss_count;
        int tlb_hit_state = tlb_hit_count;
        
        
        snprintf(filename, sizeof(filename), "output/process_%d.txt", i);
        FILE* fp = fopen(filename, "a");
        fprintf(fp, "==========================\n");fflush(fp);
        fprintf(fp, "PAGE TABLE FOR PROCESS: %2d\n", i);fflush(fp);
        fprintf(fp, "==========================\n");fflush(fp);


        fprintf(fp, "+-------------------+\n");fflush(fp);
        fprintf(fp, "|     PAGE TABLE    |\n");fflush(fp);
        fprintf(fp, "+---------+---------+\n");fflush(fp);
        fprintf(fp, "|   vpn   |   pfn   |\n");fflush(fp);
        fprintf(fp, "+---------+---------+\n");fflush(fp);

        for(int j = 0; j < process_list[i].pt_size; j++){
            fprintf(fp, "|  %4d   |  %4d   |\n", j, process_list[i].pt_ptr[j]);fflush(fp);
        }
        fprintf(fp, "+-------------------+\n\n\n");fflush(fp);

        fprintf(fp, "==============================================\n");fflush(fp);
        fprintf(fp, "GENERATING %d ADDRESSES AND TRANSLATING THEM\n", num_addresses);fflush(fp);
        fprintf(fp, "==============================================\n\n");fflush(fp);
        
        uint32_t vaddr = generate_vaddr(&process_list[i], &(process_data_list[i]));

        

        for (int j = 0; j < num_addresses; j++) {
            uint32_t paddr = translate_vaddr(&process_list[i], &(process_data_list[i]), vaddr);


            if (j < 6 * mmu.TLB->tlb_size) {

                fprintf(fp, "[GENERATE] Generating virtual address: [bin %s] - [dec %u]\n", utob(vaddr, mmu.TLB->offset_bits+mmu.TLB->vpn_bits), vaddr);fflush(fp);

                fprintf(fp, "[TRANSLATE] vaddr=%s\tvpn=%s\tpfn=%s\toffset=%s\tpaddr=%s\n",
                    utob(vaddr, mmu.TLB->offset_bits+mmu.TLB->vpn_bits),
                    utob((vaddr & mmu.TLB->vpn_mask) >> mmu.TLB->offset_bits, mmu.TLB->vpn_bits),
                    utob(process_list[i].pt_ptr[(vaddr & mmu.TLB->vpn_mask) >> mmu.TLB->offset_bits], mmu.TLB->vpn_bits),
                    utob(vaddr & mmu.TLB->offset_mask, mmu.TLB->offset_bits),
                    utob(paddr,  mmu.TLB->offset_bits+mmu.TLB->vpn_bits));
                fflush(fp);

                fprintf(fp, "[TRANSLATE] vaddr=%u\tvpn=%u\tpfn=%u\toffset=%u\tpaddr=%u\n\n",
                    vaddr,
                    (vaddr & mmu.TLB->vpn_mask) >> mmu.TLB->offset_bits,
                    process_list[i].pt_ptr[(vaddr & mmu.TLB->vpn_mask) >> mmu.TLB->offset_bits],
                    vaddr & mmu.TLB->offset_mask, 
                    paddr);
                fflush(fp);
                

                save_tlb_to_file(mmu.TLB, filename);  
            }



            vaddr = generate_vaddr_locality(vaddr, locality_of_reference, &process_list[i], &(process_data_list[i]));
        }

        int delta_hit = tlb_hit_count - tlb_hit_state;
        int delta_miss = tlb_miss_count - tlb_miss_state;
        int total = delta_hit + delta_miss;


        fprintf(fp, "==============================================\n");fflush(fp);
        fprintf(fp, "            SOME TLB STATISTICS\n");fflush(fp);
        fprintf(fp, "==============================================\n");fflush(fp);

        fprintf(fp, "\n============================\n");
        fprintf(fp, "    TLB HIT COUNT  = %d\n", delta_hit);
        fprintf(fp, "    TLB MISS COUNT = %d\n", delta_miss);
        if (total > 0) {
            fprintf(fp, "    TLB HIT RATIO  = %.2f\n", (double)delta_hit / total);
            fprintf(fp, "    TLB MISS RATIO = %.2f\n", (double)delta_miss / total);
        } else {
            fprintf(fp, "No translations performed.\n");
        }
        fprintf(fp, "============================\n");
        fflush(fp);

    }

    printf("======== CREATED %d PROCESSES ========\n\n\n\n", num_processes);


    printf("==============================================\n");
    printf("              SOME  STATISTICS\n");
    printf("==============================================\n");


    int total = tlb_hit_count + tlb_miss_count;

    printf("          TLB HIT COUNT  = %d\n", tlb_hit_count);
    printf("          TLB MISS COUNT = %d\n", tlb_miss_count);
    printf("          TLB HIT RATIO  = %.2f%%\n", 100*(double)tlb_hit_count / total);
    printf("          TLB MISS RATIO = %.2f%%\n", 100*(double)tlb_miss_count / total);
    printf("          SWAP OUT COUNT = %d\n", swap_out_count);

    
    
}

