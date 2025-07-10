#ifndef PROCESS_H
#define PROCESS_H

#include "memory.h"


typedef struct process{
    int pid;
    int* pt_ptr;
    int pt_size;

    int on_disk;
    int pt_disk_start;
    int frames_disk_start;

    uint32_t max_vaddr; // used for debugging
} process;

typedef struct process_data{
    MMU* mmu;
    int frames_requested;
} process_data;

extern process process_list[NUM_PROCESSES];
extern process_data process_data_list[NUM_PROCESSES];

process* create_process(process* p, process_data* config);

void process_destroy(process* p, process_data* data);

uint32_t generate_vaddr(process* p, process_data* data);

uint32_t generate_vaddr_locality(uint32_t base_vaddr, int locality_percent, process* p, process_data* data);

uint32_t translate_vaddr(process* p, process_data* data, uint32_t vaddr);
#endif