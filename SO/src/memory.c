#include "../include/memory.h"
#include <stdlib.h>

void page_table_print(){
    printf("\n\n--- PAGE TABLE ---\n");
    for(int i = 0; i < NUM_FRAMES_PER_PROCESS; i++){
        printf("pt entry: %d    physichal frame: %d\n", i, page_table_ptr[i]);
    }
    printf("\n\n");
}

void page_assign(int pid){

    int frames_assigned = NUM_FRAMES_PER_PROCESS;
    for(int i = 0; i < NUM_FRAMES; i++){
        //se ho assegnato tutti i frame allora esco dal ciclo
        if (frames_assigned == 0) break;

        //se il frame e libero allora lo assegno
        if (frame_bitmap[i] == 0){
            page_table_ptr[frames_assigned-1] = i;
            frame_bitmap[i] = 1;
            frames_assigned--;
            
        }
    }
}