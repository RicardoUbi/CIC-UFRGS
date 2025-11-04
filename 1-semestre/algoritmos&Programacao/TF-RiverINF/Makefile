CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -Iinclude
LDFLAGS = -lraylib -lm

SRC = $(wildcard src/*.c) $(wildcard engine/*.c)
OBJ = $(SRC:.c=.o)
BIN = river-inf

all: $(BIN)

$(BIN): $(OBJ)
	$(CC) $(OBJ) -o $@ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(BIN)

run: all
	./$(BIN)
