# Compilatore e flag
CC = gcc
CFLAGS = -Wall -Wextra -Iinclude

# File sorgenti ed eseguibile
SRC = src/global.c src/memory.c src/process.c src/pt_manager.c main.c src/tlb.c
EXEC = simulator

# Regola principale
all: $(EXEC)

$(EXEC):
	$(CC) $(CFLAGS) $(SRC) -o $(EXEC)

# Pulizia
clean:
	rm -f $(EXEC)

.PHONY: all clean
