CC = gcc
CFLAGS = -Wall -Wextra -g
LDFLAGS =

SRC_DIR = src
OBJ_DIR = build/obj
BIN_DIR = build/bin

SRCS = $(wildcard $(SRC_DIR)/*.c) \
       $(wildcard $(SRC_DIR)/container/*.c) \
       $(wildcard $(SRC_DIR)/cgroup/*.c) \
       $(wildcard $(SRC_DIR)/utils/*.c)

OBJS = $(SRCS:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)
TARGET = $(BIN_DIR)/tinydocker

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	@mkdir -p $(BIN_DIR)
	$(CC) $(OBJS) -o $@ $(LDFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)
