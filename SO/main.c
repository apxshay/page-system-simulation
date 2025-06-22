#include <stdio.h>
#include <stdlib.h>

#include "include/process.h"
#include "include/memory.h"

int main(int argv, char** argc){
    
    //creazione e inizializzazione processi
    process processi[NUM_PROCESSES];
    for (int i = 0; i < NUM_PROCESSES; i++){
        process_create(&processi[i]);
        printf("\ncreato il processo %i\n", i);

        page_assign(processi[i].pid);

        page_table_print();
    }


}