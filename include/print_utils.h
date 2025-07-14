#ifndef PRINT_UTILS_H
#define PRINT_UTILS_H

#include "memory.h"
#include "process.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

void save_tlb_to_file(const TLB* tlb, const char* filename);

void print_process_list(process* list, int num_processes);

void print_process_list_to_file(process* list, int num_processes, FILE *fp);

void load_config_from_file(const char* filename, MMU_config* config,
    int* locality_of_reference,
    int* num_processes,
    int* num_addresses);

char *utob(uint32_t n, int n_bits);

void print_ptb(PTB* blocks, int* pt_block_count);

void print_ptb_to_file(PTB* blocks, int* pt_block_count, const char* filename);

#endif 








