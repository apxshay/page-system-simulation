#ifndef PROCESS_H
#define PROCESS_H

#include "memory.h"

#define MAX_PROCESSES_ON_DISK 128


typedef struct process{
    int pid;
    int* pt_ptr;
    int pt_size;

    uint32_t max_vaddr; // used for debugging
} process;

typedef struct process_data{
    MMU* mmu;
    int frames_requested;
} process_data;

process* create_process(process* p, process_data* config);

void process_destroy(process* p, process_data* data);

uint32_t generate_vaddr(process* p, process_data* data);

uint32_t generate_vaddr_locality(uint32_t base_vaddr, int locality_percent, process* p, process_data* data);

uint32_t translate_vaddr(process* p, process_data* data, uint32_t vaddr);
#endif