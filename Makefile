# Compilatore e flag
CC = gcc
CFLAGS = -Wall -Wextra -Iinclude

# File sorgenti ed eseguibile
SRC = src/global.c src/memory.c src/process.c src/pt_manager.c main.c src/tlb.c src/print_utils.c
EXEC = simulation

# Regola principale
all: $(EXEC)
	rm -rf output/*

$(EXEC):
	$(CC) $(CFLAGS) $(SRC) -o $(EXEC)

# Pulizia
clean:
	rm -f $(EXEC)

.PHONY: all clean
