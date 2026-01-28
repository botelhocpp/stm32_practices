# Folders
BSP := bsp
HAL := $(BSP)/inc
LIB := $(BSP)/lib
BUILD := build

SRC_DIR := src
OBJ_DIR := $(BUILD)
SRCS := $(wildcard $(SRC_DIR)/*.c)
OBJS := $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRCS))

# Files
BSPLIB := $(LIB)/libbsp.a
FIRMWARE := $(BUILD)/firmware.elf

# Programs
TC := arm-none-eabi
CC := $(TC)-gcc
OBJCPY := $(TC)-objcopy
OBJDMP := $(TC)-objdump
GDB := $(TC)-gdb
FLASHER := openocd

# Flags
CFLAGS := -mcpu=cortex-m3 -mthumb -nostdlib -I$(HAL) -Wall -g -O0
LDFLAGS = -T $(BSP)/linker.ld  -nostartfiles -L$(LIB) -lbsp

# TARGETS

.PHONY: all build flash clean debug server terminal

all: build flash

$(FIRMWARE): $(BSPLIB) $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BSPLIB):
	make all -C $(BSP)/

# ACTIONS

build: $(FIRMWARE) 

flash: $(FIRMWARE)
	$(FLASHER) -f interface/stlink.cfg -f target/stm32f1x.cfg -c "program $< verify reset exit"

debug: $(FIRMWARE)
	$(GDB) -ex "target remote localhost:3333" -ex "monitor reset halt" -ex "break main" -ex "continue" -ex "load" $<

clean:
	rm -f *.o $(BUILD)/*
	make clean -C $(BSP)/ 

server:
	openocd -f interface/stlink.cfg -f target/stm32f1x.cfg

terminal:
	telnet localhost 4444
