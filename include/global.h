#ifndef GLOBAL_H
#define GLOBAL_H

#include <stdint.h>

#define ACTUAL_RAM_SIZE 4096
#define FRAME_SIZE 64
#define NUM_PROCESSES 20


#define RAM_SIZE (ACTUAL_RAM_SIZE + FRAME_SIZE)
#define DISK_SIZE (2 * RAM_SIZE)
#define PAGE_TABLE_PTR ACUTAL_RAM_SIZE
#define NUM_FRAMES (ACTUAL_RAM_SIZE / FRAME_SIZE)
#define TLB_SIZE (NUM_FRAMES / 8)


#define TLB_MISS 0xFFFFFFFF
#define LOCALITY_PERC 90
#define NUM_ADDRESSES 1000


extern int* page_table_ptr;
extern int process_global_cnt;
extern int active_process_cnt;
extern int tlb_miss_count;
extern int tlb_hit_count;
extern int swap_out_count;

#endif
