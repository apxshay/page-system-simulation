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
    config.ram_size = RAM_SIZE;
    config.max_pt_blocks = RAM_SIZE / 2;
    config.num_frames = NUM_FRAMES;

    mmu_init(&mmu, &config);
    process* p[10];
    
    for (int i = 0; i < 10; i++){
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
        printf("printing p_data to check: frames requested: %d\n", p_data.frames_requested);
        
        printf("created process %d: requested frames: %d, start in memory: %d\nmemory situation:\n", p[i]->pid, p[i]->pt_size, (int)(p[i]->pt_ptr - p_data.mmu->RAM));
        print(mmu.pt_blocks, &(mmu.pt_block_count));

        if (frames < 5){
            printf("destroyed process %d\n", p[i]->pid);
            process_destroy(p[i], &p_data);
            p[i] = NULL; 
            print(mmu.pt_blocks, &(mmu.pt_block_count));
        }
    }

    for (int i = 0; i < 10; i++){
        if (p[i] == NULL) continue;
        printf("page table for process %d\n", p[i]->pid);
        for (int j = 0; j < p[i]->pt_size; j++){
            printf("page idx: %d --> frame: %d\n", j, (p[i]->pt_ptr)[j]);
        }
    }


}