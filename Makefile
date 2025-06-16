CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -DVERSION=\"$(VERSION)\"
VERSION = 0.1.0

SRC_DIR = src
BUILD_DIR = build
OBJ_DIR = $(BUILD_DIR)/obj
BIN_DIR = $(BUILD_DIR)/bin

# Get all .c files from src directory and its subdirectories
SRCS = $(wildcard $(SRC_DIR)/*.c) \
       $(wildcard $(SRC_DIR)/container/*.c) \
       $(wildcard $(SRC_DIR)/cgroup/*.c) \
       $(wildcard $(SRC_DIR)/cli/*.c) \
       $(wildcard $(SRC_DIR)/utils/*.c)

# Convert source files to object files
OBJS = $(SRCS:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

# Main binary name with version
BIN_NAME = tinydocker-$(VERSION)

.PHONY: all clean debug release

all: debug

# Debug build (default)
debug: CFLAGS += -g -DDEBUG
debug: $(BIN_DIR)/tinydocker

# Release build
release: CFLAGS += -O2
release: $(BIN_DIR)/$(BIN_NAME)

# Create necessary directories
$(OBJ_DIR) $(BIN_DIR):
	mkdir -p $@

# Compile source files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# Link debug binary
$(BIN_DIR)/tinydocker: $(OBJS) | $(BIN_DIR)
	$(CC) $(OBJS) -o $@

# Link release binary
$(BIN_DIR)/$(BIN_NAME): $(OBJS) | $(BIN_DIR)
	$(CC) $(OBJS) -o $@

clean:
	rm -rf $(BUILD_DIR)
