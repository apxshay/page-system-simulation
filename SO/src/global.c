#include "../include/global.h"
#include <string.h>

int RAM[RAM_SIZE];
int DISK[DISK_SIZE];
int frame_bitmap[NUM_FRAMES];
int* page_table_ptr = RAM + PAGE_TABLE_PTR;
int process_global_cnt = 0;

void __init__(){
    memset(RAM, 0, sizeof(RAM));
    memset(frame_bitmap, 0, sizeof(frame_bitmap));
    process_global_cnt = 0;
}