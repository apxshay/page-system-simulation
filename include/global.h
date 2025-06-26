#ifndef GLOBAL_H
#define GLOBAL_H

#include <stdint.h>

#define ACUTAL_RAM_SIZE 4096
#define FRAME_SIZE 64
#define NUM_PROCESSES 20

#define RAM_SIZE (ACUTAL_RAM_SIZE + FRAME_SIZE)
#define DISK_SIZE (2 * RAM_SIZE)
#define PAGE_TABLE_PTR ACUTAL_RAM_SIZE
#define NUM_FRAMES (ACUTAL_RAM_SIZE / FRAME_SIZE)
#define PAGE_TABLE_ENTRIES (NUM_FRAMES_PER_PROCESS)



extern int* page_table_ptr;
extern int process_global_cnt;
extern int active_process_cnt;



#endif
