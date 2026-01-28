# Folders
BSP := bsp
SRC := src
BIN := bin
BUILD := build

# Programs
TC := arm-none-eabi
CC := $(TC)-gcc
OBJCPY := $(TC)-objcopy
OBJDMP := $(TC)-objdump
GDB := $(TC)-gdb
FLASHER := openocd

# Flags
CFLAGS := -mcpu=cortex-m3 -mthumb -nostdlib -I$(BSP) -Wall -g
LDFLAGS = -T $(BSP)/linker.ld  -nostartfiles

# TARGETS

.PHONY: all flash clean debug server terminal

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

flash: $(BUILD)/blinky.elf
	$(FLASHER) -f interface/stlink.cfg -f target/stm32f1x.cfg -c "program $< verify reset exit"

debug: $(BUILD)/blinky.elf
	$(GDB) -ex "target remote localhost:3333" -ex "monitor reset halt" -ex "break main" -ex "continue" -ex "load" $<

clean:
	rm -f *.o $(BUILD)/* $(BIN)/*

server:
	openocd -f interface/stlink.cfg -f target/stm32f1x.cfg

terminal:
	telnet localhost 4444
