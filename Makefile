CC := gcc
CFLAGS := -Wall -Wextra -Wshadow -Werror -std=c23 -Wconversion -pedantic -MMD -MP -Isrc -Iinclude
LDFLAGS =
TARGET := build/httpserver

SRC = $(shell find src -type f -name "*.c")
OBJ = $(patsubst src/%.c, build/%.o, $(SRC))
DEP = $(OBJ:.o=.d)

.PHONY: all clean run run-port

all: $(TARGET)

$(TARGET): $(OBJ)
		$(CC) $(OBJ) -o $(TARGET) $(LDFLAGS)

build/%.o: src/%.c
		@mkdir -p $(dir $@)
		$(CC) $(CFLAGS) -c $< -o $@

-include $(DEP)

run: all
	./$(TARGET)

clean:
		rm -rf build
