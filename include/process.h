#ifndef PROCESS_H
#define PROCESS_H

#include "memory.h"

#define MAX_PROCESSES_ON_DISK 128


typedef struct process{
    int pid;

    int* pt_ptr;
    int pt_size;
} process;

typedef struct process_data{
    MMU* mmu;
    int frames_requested;
} process_data;

process* create_process(process* p, process_data* config);

void process_destroy(process* p, process_data* data);

#endif