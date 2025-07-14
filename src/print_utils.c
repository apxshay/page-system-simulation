#include "../include/print_utils.h"

void save_tlb_to_file(const TLB* tlb, const char* filename) {
    FILE* fp = fopen(filename, "a");
    if (fp == NULL) {
        perror("Error opening file for writing");
        return;
    }

    fprintf(fp, "+------------------------------------+\n");fflush(fp);
    fprintf(fp, "|                 TLB                |\n");fflush(fp);
    fprintf(fp, "+-------+-------+-------+------------+\n");fflush(fp);
    fprintf(fp, "| VPN   | PFN   | Valid | Referenced |\n");fflush(fp);
    fprintf(fp, "+-------+-------+-------+------------+\n");fflush(fp);

    for (int i = 0; i < tlb->tlb_size; ++i) {
        TLBEntry entry = tlb->entries[i];
        fprintf(fp, "| %5u | %5u | %5d | %10d |\n",
                entry.vpn, entry.pfn, entry.valid, entry.referenced);
    }

    fprintf(fp, "+------------------------------------+\n\n");fflush(fp);

    fclose(fp);
}

void print_process_list(process* list, int num_processes) {
    printf("=== Process List ===\n");
    for (int i = 0; i < num_processes; i++) {
        process p = list[i];
        printf("    PID: %d\n", p.pid);
        printf("      Page Table Size: %d\n", p.pt_size);
        printf("      On Disk: %s\n", p.on_disk ? "Yes" : "No");
        printf("      PT Disk Start: %d\n", p.pt_disk_start);
        printf("      Frames Disk Start: %d\n", p.frames_disk_start);
        printf("      Max Virtual Address: %u\n", p.max_vaddr);

        printf("  Page Table : [ ");
        if (p.pt_ptr != NULL) {
            for (int j = 0; j < p.pt_size; j++) {
                printf("%d ", p.pt_ptr[j]);
            }
        } else {
            printf("NULL");
        }
        printf("]\n");
    }
    printf("====================\n");
}

void print_process_list_to_file(process* list, int num_processes, FILE *fp) {
    
    fprintf(fp, "=== Process List ===\n");
    for (int i = 0; i < num_processes; i++) {
        process p = list[i];
        fprintf(fp, "PID: %d\n", p.pid);
        fprintf(fp, "  Page Table Size: %d\n", p.pt_size);
        fprintf(fp, "  On Disk: %s\n", p.on_disk ? "Yes" : "No");
        fprintf(fp, "  PT Disk Start: %d\n", p.pt_disk_start);
        fprintf(fp, "  Frames Disk Start: %d\n", p.frames_disk_start);
        fprintf(fp, "  Max Virtual Address: %u\n", p.max_vaddr);

        fprintf(fp, "  Page Table : [ ");
        if (p.pt_ptr != NULL) {
            for (int j = 0; j < p.pt_size; j++) {
                fprintf(fp, "%d ", p.pt_ptr[j]);
            }
        } else {
            fprintf(fp, "NULL");
        }
        fprintf(fp, "]\n");
    }
    fprintf(fp, "====================\n");
}

void load_config_from_file(const char* filename, MMU_config* config,
    int* locality_of_reference,
    int* num_processes,
    int* num_addresses) {
    FILE* fp = fopen(filename, "r");
    if (!fp) {
    perror("Could not open config file");
    exit(EXIT_FAILURE);
    }

    char line[256];
    while (fgets(line, sizeof(line), fp)) {
        
        if (line[0] == '#' || line[0] == '\n') continue;

        char key[128];
        int value;

        if (sscanf(line, "%127[^=]=%d", key, &value) == 2) {
            if (strcmp(key, "DISK_SIZE") == 0) {
                config->disk_size = value;
            } else if (strcmp(key, "RAM_SIZE") == 0) {
                config->ram_size = value;
            } else if (strcmp(key, "NUM_FRAMES") == 0) {
                config->num_frames = value;
                config->max_pt_blocks = value; 
            } else if (strcmp(key, "TLB_SIZE") == 0) {
                config->tlb_size = value;
            } else if (strcmp(key, "TLB_REPLACEMENT_POLICY") == 0) {
                config->tlb_replacement_policy = value == 1 ? TLB_REPL_POLICY_FIFO : TLB_REPL_POLICY_CLOCK;
            } else if (strcmp(key, "LOCALITY_PERC") == 0) {
                *locality_of_reference = value;
            } else if (strcmp(key, "NUM_PROCESSES") == 0) {
                *num_processes = value;
            } else if (strcmp(key, "NUM_ADDRESSES") == 0) {
                *num_addresses = value;
            } else {
                printf("Unknown key: %s\n", key);
            }
        }
    }

    fclose(fp);
}

char *utob(uint32_t n, int n_bits) {
    // n_bits bit + '\0'
    char *bin_str = malloc(n_bits+1);
    if (!bin_str) {
        return NULL; // allocation error
    }

    bin_str[n_bits] = '\0'; 

    for (int i = n_bits-1; i >= 0; i--) {
        bin_str[i] = (n & 1) ? '1' : '0';
        n >>= 1;
    }

    return bin_str;
}

void print_ptb(PTB* blocks, int* pt_block_count) {
    
    qsort(blocks, *pt_block_count, sizeof(PTB), compare_by_start);

    printf("\n=== Page Table Blocks Allocation ===\n");
    printf("Total blocks: %d\n", *pt_block_count);
    printf("Legend: PID = -1 means free block.\n\n");

    printf("+--------+--------+--------+------+\n");
    printf("|  PID   | Start  | Size   | Used |\n");
    printf("+--------+--------+--------+------+\n");

    for (int i = 0; i < *pt_block_count; i++) {
        printf("| %6d | %6d | %6d |  %3s |\n",
            blocks[i].pid,
            blocks[i].start,
            blocks[i].size,
            blocks[i].used ? "Yes" : "No"
        );
    }

    printf("+--------+--------+--------+------+\n");
    printf("\n");
}

void print_ptb_to_file(PTB* blocks, int* pt_block_count, const char* filename) {
    
    FILE* fp = fopen(filename, "a");
    if (fp == NULL) {
        perror("Error opening file");
        return;
    }

    
    qsort(blocks, *pt_block_count, sizeof(PTB), compare_by_start);

    fprintf(fp, "\n=== Page Table Blocks Allocation ===\n");
    fprintf(fp, "Total blocks: %d\n", *pt_block_count);
    fprintf(fp, "Legend: PID = -1 means free block.\n\n");

    fprintf(fp, "+--------+--------+--------+------+\n");
    fprintf(fp, "|  PID   | Start  | Size   | Used |\n");
    fprintf(fp, "+--------+--------+--------+------+\n");

    for (int i = 0; i < *pt_block_count; i++) {
        fprintf(fp, "| %6d | %6d | %6d |  %3s |\n",
            blocks[i].pid,
            blocks[i].start,
            blocks[i].size,
            blocks[i].used ? "Yes" : "No"
        );
    }

    fprintf(fp, "+--------+--------+--------+------+\n");
    fprintf(fp, "\n");

    fflush(fp);

    fclose(fp);

}