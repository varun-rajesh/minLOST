CC = gcc
CFLAGS = -Iinclude -Werror -Wall -O3 -MMD -MP
LDFLAGS = 

SRC_DIR = src
INC_DIR = include
BUILD_DIR = build

SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(SRCS))
DEPS = $(OBJS:.o=.d)
TARGET = star_tracker

RM = rm -rf
MKDIR = mkdir -p
PYTHON = python

ifeq ($(OS),Windows_NT)
    RM = del /Q /S
    MKDIR = mkdir
    PYTHON = python
    TARGET = star_tracker.exe
else
    LDFLAGS += -lm
endif

all: init $(TARGET)

init:
	$(PYTHON) init.py config.ini

$(TARGET): $(OBJS)
	$(CC) $(OBJS) $(LDFLAGS) -o $(TARGET)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

-include $(DEPS)

$(BUILD_DIR):
	$(MKDIR) $(BUILD_DIR)

clean:
	$(RM) $(BUILD_DIR) $(TARGET)

rebuild: clean all

.PHONY: all clean rebuild init
