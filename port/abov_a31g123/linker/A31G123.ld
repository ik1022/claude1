/**
 * @file A31G123.ld
 * @brief ABOV A31G123 Linker Script
 *
 * Memory Layout for ABOV A31G123 MCU:
 * - Flash: 0x00000000 - 0x0001FFFF (128 KB)
 * - RAM:   0x20000000 - 0x20007FFF (32 KB)
 * - System: 0xE0000000 - 0xE00FFFFF (Cortex-M0+ core peripherals)
 *
 * Sections:
 * .text   - Executable code (Flash)
 * .data   - Initialized data (Flash, copied to RAM at boot)
 * .bss    - Uninitialized data (RAM)
 * .heap   - Dynamic memory (RAM)
 * .stack  - Call stack (RAM, grows downward)
 */

/* ============================================================================
   Memory Definition
   ============================================================================ */

MEMORY
{
    /* Flash: 128KB @ 0x00000000 */
    FLASH (rx) : ORIGIN = 0x00000000, LENGTH = 128K

    /* RAM: 32KB @ 0x20000000 */
    RAM (rwx) : ORIGIN = 0x20000000, LENGTH = 32K

    /* System (read-only, core peripherals) */
    SYSTEM (r) : ORIGIN = 0xE0000000, LENGTH = 1M
}

/* ============================================================================
   Entry Point
   ============================================================================ */

ENTRY(Reset_Handler)

/* ============================================================================
   Section Definitions
   ============================================================================ */

SECTIONS
{
    /* ========================================================================
       Flash Sections (Code and Read-Only Data)
       ======================================================================== */

    .text : ALIGN(4)
    {
        /* Interrupt vector table */
        KEEP(*(.vectors))

        /* Code */
        *(.text)
        *(.text.*)
        *(.rodata)
        *(.rodata.*)

        /* C++ support */
        *(.preinit_array)
        *(.init_array)
        *(.fini_array)

        /* Exception handling (if enabled) */
        *(.eh_frame)
        *(.gcc_except_table)

        /* Ensure alignment */
        . = ALIGN(4);
        _etext = .;
    } > FLASH

    /* Read-only data that must be in flash */
    .rodata : ALIGN(4)
    {
        *(.rodata)
        *(.rodata.*)
        . = ALIGN(4);
    } > FLASH

    /* ========================================================================
       RAM Sections (Initialized and Uninitialized Data)
       ======================================================================== */

    .data : ALIGN(4)
    {
        _sdata = .;                 /* Start of data section in RAM */
        *(.data)                    /* Initialized data */
        *(.data.*)
        . = ALIGN(4);
        _edata = .;                 /* End of data section in RAM */
    } > RAM AT > FLASH

    /* Copy table for .data section
       Linker will copy from _sidata (in Flash) to _sdata (in RAM) */
    _sidata = LOADADDR(.data);

    /* Uninitialized data (will be zeroed at startup) */
    .bss (NOLOAD) : ALIGN(4)
    {
        _sbss = .;                  /* Start of BSS section */
        *(.bss)
        *(.bss.*)
        *(COMMON)
        . = ALIGN(4);
        _ebss = .;                  /* End of BSS section */
    } > RAM

    /* ========================================================================
       Heap and Stack
       ======================================================================== */

    /* Heap: starts after BSS, grows upward */
    .heap (NOLOAD) : ALIGN(8)
    {
        _sheap = .;
        . += 4K;                    /* 4KB heap space */
        _eheap = .;
    } > RAM

    /* Stack: at end of RAM, grows downward */
    .stack (NOLOAD) : ALIGN(8)
    {
        . += 2K;                    /* 2KB stack space */
        _estack = .;                /* Stack pointer (grows downward) */
    } > RAM

    /* ========================================================================
       Debug Sections
       ======================================================================== */

    /* Symbols for memory usage analysis */
    _flash_start = ORIGIN(FLASH);
    _flash_end = ORIGIN(FLASH) + LENGTH(FLASH);
    _flash_size = LENGTH(FLASH);

    _ram_start = ORIGIN(RAM);
    _ram_end = ORIGIN(RAM) + LENGTH(RAM);
    _ram_size = LENGTH(RAM);

    /* ========================================================================
       Garbage Collection (remove unused sections)
       ======================================================================== */

    /DISCARD/ :
    {
        *(.gnu.lto_*)              /* LTO intermediate data */
        *(.ARM.exidx)              /* ARM unwind tables (not used in M0+) */
        *(.ARM.extab)
        *(.eh_frame)
    }

    /* ========================================================================
       Misc Sections
       ======================================================================== */

    .ARM.attributes 0 :
    {
        *(.ARM.attributes)
    }

    .comment 0 :
    {
        *(.comment)
    }

    .debug 0 :
    {
        *(.debug)
    }
}

/* ============================================================================
   Linker Symbols for Startup Code
   ============================================================================ */

/* These symbols are used by the startup code to initialize memory */
PROVIDE(__etext = _etext);
PROVIDE(__data_start__ = _sdata);
PROVIDE(__data_end__ = _edata);
PROVIDE(__data_load_start = _sidata);
PROVIDE(__bss_start__ = _sbss);
PROVIDE(__bss_end__ = _ebss);
PROVIDE(__StackTop = _estack);
PROVIDE(__StackLimit = _estack - 2K);
PROVIDE(__HeapBase = _sheap);
PROVIDE(__HeapLimit = _eheap);

/* ============================================================================
   Memory Check
   ============================================================================ */

/* Ensure we don't overflow memory regions
   (These will generate linker errors if memory is exceeded) */

ASSERT(_estack <= ORIGIN(RAM) + LENGTH(RAM), "Stack would overflow RAM!")
ASSERT(_eheap <= _estack, "Heap would overlap stack!")
ASSERT(_ebss <= _sheap, "BSS would overflow into heap!")
