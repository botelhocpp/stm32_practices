LIB := lib

TC := arm-none-eabi
CC := $(TC)-gcc
AR := $(TC)-ar
CFLAGS := -mcpu=cortex-m3 -mthumb -nostdlib -Iinc -Wall

SRC_DIR := src
OBJ_DIR := build
SRCS := $(wildcard $(SRC_DIR)/*.c)
OBJS := $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRCS))

.PHONY: all clean

all: $(LIB)/libbsp.a

$(LIB)/libbsp.a: $(OBJS)
	$(AR) rcs $@ $^

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm build/* lib/*
