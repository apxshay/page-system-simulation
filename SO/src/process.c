#include "../include/process.h"
#include "../include/memory.h"
#include <stdlib.h>

process* process_create(process* p){

    p = malloc(sizeof(process));
    p->pid = process_global_cnt++;

    return p;
}

void process_destroy(process* p){
    free(p);
    process_global_cnt--;
}