/*
 * arc32.ld  —  Arc32 Linker Script
 *
 * THEJAS32 memory map (from datasheet Table 4):
 *   SRAM: 0x0020_0000 - 0x0023_FFFF  (256 KB)
 *
 * Boot modes (per datasheet section 2.2.5):
 *   - UART boot:      bootloader loads binary into SRAM via XMODEM, jumps to 0x00200000
 *   - SPI Flash boot: bootloader copies first 250KB from flash into SRAM, jumps to 0x00200000
 *
 * Either way, OUR code always starts execution at SRAM base 0x00200000.
 * There is no separate "flash address space" to link against — the flash
 * is just a backing store that gets copied into SRAM by the boot ROM.
 */

OUTPUT_ARCH("riscv")
ENTRY(_start)

MEMORY
{
    /* Reserve the first 256 bytes for boot vector compatibility (unused
       in user code but keeps a clean offset for future bootloader hooks) */
    SRAM (rwx) : ORIGIN = 0x00200000, LENGTH = 256K
}

/* Stack size: 8 KB (adjust if sketches need more for deep recursion) */
STACK_SIZE = 8K;

SECTIONS
{
    /* ---- Boot / vector code first ---- */
    .init :
    {
        *(.init)
    } > SRAM

    /* ---- Code ---- */
    .text :
    {
        *(.text)
        *(.text.*)
        *(.rodata)
        *(.rodata.*)
        *(.srodata)
        *(.srodata.*)
        . = ALIGN(4);
    } > SRAM

    /* ---- C++ static constructors ---- */
    .init_array :
    {
        PROVIDE_HIDDEN(__init_array_start = .);
        KEEP(*(.init_array.*))
        KEEP(*(.init_array))
        PROVIDE_HIDDEN(__init_array_end = .);
    } > SRAM

    /* ---- Global pointer (for gp-relative addressing relaxation) ---- */
    . = ALIGN(8);
    PROVIDE(__global_pointer$ = . + 0x800);

    /* ---- Initialized data: stored here in SRAM directly since we
            execute-in-place from SRAM (no separate flash XIP) ---- */
    _data_lma_start = .;
    .data :
    {
        _data_start = .;
        *(.data)
        *(.data.*)
        *(.sdata)
        *(.sdata.*)
        . = ALIGN(4);
        _data_end = .;
    } > SRAM
    _data_lma_end = _data_lma_start + (_data_end - _data_start);

    /* ---- Zero-initialized data ---- */
    .bss :
    {
        _bss_start = .;
        *(.bss)
        *(.bss.*)
        *(.sbss)
        *(.sbss.*)
        *(COMMON)
        . = ALIGN(4);
        _bss_end = .;
    } > SRAM

    /* ---- Heap (grows upward from end of .bss) ---- */
    . = ALIGN(8);
    _heap_start = .;

    /* ---- Stack (placed at top of SRAM, grows downward) ---- */
    _stack_top = ORIGIN(SRAM) + LENGTH(SRAM) - 4;
    _heap_end  = _stack_top - STACK_SIZE;

    /DISCARD/ :
    {
        *(.comment)
        *(.eh_frame)
    }
}

/* Sanity check: ensure heap doesn't collide with stack at link time.
   (Actual runtime overflow is the sketch author's responsibility — no MMU.) */
ASSERT(_heap_end > _bss_end, "Arc32: SRAM exhausted — reduce sketch size or globals")
