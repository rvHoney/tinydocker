CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -Wvla -pedantic -Werror
SRC_DIR = src
OBJ_DIR = build/obj
BIN_DIR = build
BIN = $(BIN_DIR)/tinydocker
SRC = $(wildcard $(SRC_DIR)/*.c)
OBJ = $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRC))

all: $(BIN)

debug: CFLAGS += -g -O0
debug: $(BIN)

$(BIN): $(OBJ) | $(BIN_DIR)
	$(CC) $(CFLAGS) -o $@ $^

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BIN_DIR) $(OBJ_DIR):
	mkdir -p $@

clean:
	rm -rf build/

.PHONY: all clean debug
