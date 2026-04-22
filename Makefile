CC = gcc
CFLAGS = -Wall -Werror -pthread
DEBUG_FLAGS = -g -O0
SRC = biceps.c creme.c list.c
OBJ = $(SRC:.c=.o)
BIN = biceps

all: $(BIN)

$(BIN): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

memory-leak:
	$(CC) $(CFLAGS) $(DEBUG_FLAGS) -o biceps-memory-leaks $(SRC)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) biceps biceps-memory-leaks

.PHONY: all clean memory-leak