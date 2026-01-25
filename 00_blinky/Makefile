# Folders
BSP := bsp
SRC := src
BIN := bin
BUILD := build

# Programs
TC := arm-none-eabi
CC := $(TC)-gcc
OBJCOPY := $(TC)-objcopy

# Flags
CFLAGS := -mcpu=cortex-m3 -mthumb -nostdlib -I$(BSP) -Wall
LDFLAGS = -T $(BSP)/linker.ld  -nostartfiles

# TARGETS

.PHONY: all flash clean

all: $(BIN)/blinky.bin

$(BUILD)/blinky.elf: $(BUILD)/startup.o $(BUILD)/main.o
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@

$(BUILD)/main.o: $(SRC)/main.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD)/startup.o: $(BSP)/startup.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BIN)/blinky.bin: $(BUILD)/blinky.elf
	$(OBJCOPY) -O binary $< $@

# ACTIONS

flash: $(BIN)/blinky.bin
	st-flash write $(BIN)/blinky.bin 0x08000000

clean:
	rm -f *.o $(BUILD)/* $(BIN)/*
