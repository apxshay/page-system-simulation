#ifndef PROCESS_H
#define PROCESS_H

#define MAX_PROCESSES_ON_DISK 128


typedef struct process{
    int pid;
} process;

process* process_create(process* p);

void process_destroy(process* p);

#endif