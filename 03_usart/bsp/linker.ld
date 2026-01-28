ENTRY(_start);

MEMORY
{
    FLASH (rx) : ORIGIN = 0x08000000, LENGTH = 64K
    SRAM (rwx) : ORIGIN = 0x20000000, LENGTH = 20K
}

_estack = ORIGIN(SRAM) + LENGTH(SRAM);

SECTIONS
{
    .text : 
    {
        . = ALIGN(4);
        KEEP(*(.vectors))
        *(.text)
        *(.rodata)
    } > FLASH

    _sidata = LOADADDR(.data);

    .data : 
    {
        _sdata = .;
        *(.data)
        _edata = .;
    } > SRAM AT > FLASH

    .bss :
    {
        _sbss = .;
        *(.bss)
        _ebss = .;
    } > SRAM
}