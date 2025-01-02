# Compiler and flags
CC = gcc
CFLAGS = -Iinclude -Werror -Wall -O3 -MMD -MP
LDFLAGS = 

# Folders
SRC_DIR = src
INC_DIR = include
BUILD_DIR = build

# Files
SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(SRCS))
DEPS = $(OBJS:.o=.d)
TARGET = star_tracker

# OS-specific commands
RM = rm -rf
MKDIR = mkdir -p
PYTHON = python

# Detect platform
ifeq ($(OS),Windows_NT)
    RM = del /Q /S
    MKDIR = mkdir
    PYTHON = python
    TARGET = star_tracker.exe  # Ensure .exe extension on Windows
endif

# Default target
all: init $(TARGET)

# Run Python initialization script
init:
	$(PYTHON) init.py

# Build target
$(TARGET): $(OBJS)
	$(CC) $(LDFLAGS) $^ -o $@

# Compile source files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Include dependency files
-include $(DEPS)

# Create build directory if it doesn't exist
$(BUILD_DIR):
	$(MKDIR) $(BUILD_DIR)

# Clean up
clean:
	$(RM) $(BUILD_DIR) $(TARGET)

# Rebuild everything
rebuild: clean all

# Phony targets
.PHONY: all clean rebuild init
