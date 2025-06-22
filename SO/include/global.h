#ifndef GLOBAL_H
#define GLOBAL_H

#include <stdint.h>

#define NUM_FRAMES_PER_PROCESS 4
#define RAM_SIZE (4096 + NUM_FRAMES_PER_PROCESS)
#define DISK_SIZE 8192
#define PAGE_TABLE_PTR 4096
#define FRAME_SIZE 64
#define NUM_FRAMES ((RAM_SIZE-NUM_FRAMES_PER_PROCESS) / FRAME_SIZE)
#define PAGE_TABLE_ENTRIES 64
#define NUM_PROCESSES 20

extern int RAM[RAM_SIZE];
extern int DISK[DISK_SIZE];
extern int frame_bitmap[NUM_FRAMES];
extern int* page_table_ptr;
extern int process_global_cnt;

void __init__();

#endif
