APPLICATION := factors

# Directories
BUILD_DIR := .
SRC_DIR := .
INC_DIR := .
OBJ_DIR := objects
TEST_DIR := tests
TMP_DIR := .tmp

# Make flags
MAKEFLAGS += -s
SHELL := /bin/bash

# Compiler flags
CC := gcc
CFLAGS := -Wall -L/usr/local/gmp -lgmp -pthread -O3 # -ggdb3 # -flto -finline-functions -ftree-vectorize -fomit-frame-pointer

# Files
SOURCE_FILES := $(wildcard $(SRC_DIR)/*.c)
HEADER_FILES := $(wildcard $(INC_DIR)/*.h)
OBJECT_FILES := $(patsubst %.c, $(OBJ_DIR)/%.o, $(SOURCE_FILES))
DEP_FILES := $(OBJECT_FILES:.o=.d)
EXECUTABLE := $(BUILD_DIR)/$(APPLICATION)

# Tests
INTEGRATION_TESTS_FILES = $(patsubst $(TEST_DIR)/integration/%.py,%,$(wildcard $(TEST_DIR)/integration/*.py))
UNIT_TEST_FILES = $(patsubst %.c, %, $(notdir $(wildcard $(TEST_DIR)/unit/*.c)))

all: clear_screen build run

build: setup_dirs $(EXECUTABLE)
	@$(MAKE) announce MESSAGE="Compiled successfully"

$(EXECUTABLE): $(OBJECT_FILES)
	$(CC) $(CFLAGS) -I${INC_DIR} -I${SRC_DIR} $^ -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -I${INC_DIR} -I${SRC_DIR} -MMD -c $^ -o $@

-include $(DEPS)

run:
	@$(MAKE) announce MESSAGE="Running $(APPLICATION)"
	time ./$(EXECUTABLE) $(TEST_DIR)/factors/all.txt

unit_tests: $(UNIT_TEST_FILES)

$(UNIT_TEST_FILES): %: $(TEST_DIR)/unit/%.c
	$(CC) $(filter-out ./monty.c, $(SOURCE_FILES)) $< -o $(TMP_DIR)/$@.o
	@if ./$(TMP_DIR)/$@.o 2>&1 >/dev/null; then \
		echo "Test $@ passed"; \
	else \
		echo "Test $@ failed"; \
		./$(TMP_DIR)/$@.o; \
	fi; \

run_tests: setup_dirs
	@$(MAKE) announce MESSAGE="Running unit tests"
	@$(MAKE) -k -j 8 unit_tests

setup_dirs:
	@mkdir -p $(OBJ_DIR)
	@mkdir -p ./$(BUILD_DIR)
	@mkdir -p ./$(TEST_DIR)
	@mkdir -p ./$(TEST_DIR/integration)
	@mkdir -p ./$(TEST_DIR/unit)
	@mkdir -p ./$(TMP_DIR)

announce:
	@echo "------------------------------------------"
	@printf "|%*s%s%*s|\n" $$(expr 20 - $${#MESSAGE} / 2) "" "$(MESSAGE)" $$(expr 20 - $$(($${#MESSAGE} + 1)) / 2) ""
	@echo "------------------------------------------"

check_memory:
	@$(MAKE) announce MESSAGE="Checking memory leaks"
	@valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./$(EXECUTABLE) $(TEST_DIR)/factors/all.txt

clear_screen:
	@clear

clean:
	rm -rf $(OBJ_DIR)
	rm $(EXECUTABLE)
