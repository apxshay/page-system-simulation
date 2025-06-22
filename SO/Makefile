# Compilatore e flag
CC = gcc
CFLAGS = -Wall -Wextra -Iinclude

# File
SRC = src/global.c src/memory.c src/process.c main.c
OBJ = $(SRC:.c=.o)
EXEC = simulator

# Regole
all: $(EXEC)

$(EXEC): $(OBJ)
	$(CC) $(OBJ) -o $@

# Regole per .o
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Pulizia
clean:
	rm -f $(SRC:.c=.o) $(EXEC)

.PHONY: all clean
