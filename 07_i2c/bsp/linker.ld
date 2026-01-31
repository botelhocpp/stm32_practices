ENTRY(_start);

HEAP_SIZE  = DEFINED(__heap_size__)  ? __heap_size__  : 512;
STACK_SIZE = DEFINED(__stack_size__) ? __stack_size__ : 1K;

MEMORY
{
    FLASH (rx) : ORIGIN = 0x08000000, LENGTH = 64K
    SRAM (rwx) : ORIGIN = 0x20000000, LENGTH = 20K
}

SECTIONS
{
    .text : 
    {
        . = ALIGN(4);
        KEEP(*(.vectors))
        *(.text)
        *(.text*)
        *(.rodata)
        *(.rodata*)
        *(.glue_7)         /* glue arm to thumb code */
        *(.glue_7t)        /* glue thumb to arm code */

        KEEP (*(.init))
        KEEP (*(.fini))
    } > FLASH

    .preinit_array :
    {
        PROVIDE_HIDDEN (__preinit_array_start = .);
        KEEP (*(.preinit_array*))
        PROVIDE_HIDDEN (__preinit_array_end = .);
    } > FLASH

    .init_array :
    {
        PROVIDE_HIDDEN (__init_array_start = .);
        KEEP (*(SORT(.init_array.*)))
        KEEP (*(.init_array*))
        PROVIDE_HIDDEN (__init_array_end = .);
    } > FLASH

    .fini_array :
    {
        PROVIDE_HIDDEN (__fini_array_start = .);
        KEEP (*(SORT(.fini_array.*)))
        KEEP (*(.fini_array*))
        PROVIDE_HIDDEN (__fini_array_end = .);
    } > FLASH

    _sidata = LOADADDR(.data);

    .data : 
    {
        . = ALIGN(4);
        _sdata = .;
        *(.data)
        *(.data*)
        . = ALIGN(4);
        _edata = .;
    } > SRAM AT > FLASH

    .bss :
    {
        . = ALIGN(4);
        _sbss = .;
        __bss_start__ = _sbss;
        *(.bss)
        *(.bss*)
        *(COMMON)
        . = ALIGN(4);
        _ebss = .;
        __bss_end__ = _ebss;
    } > SRAM

  .heap :
  {
    . = ALIGN(8);
    _mem_end = .;
    PROVIDE(end = .);
    _heap_base = .;
    . += HEAP_SIZE;
    _heap_limit = .;
  } > SRAM

  _stack_top   = ORIGIN(SRAM) + LENGTH(SRAM);
  _stack_limit = _stack_top - STACK_SIZE;

  .stack _stack_limit :
  {
    . = ALIGN(8);
    _stack_base = .;
    . += STACK_SIZE;
  } > SRAM

  PROVIDE(_estack = _stack_top);

  ASSERT(_stack_limit >= _heap_limit, "region SRAM overflowed with stack and heap")
}