/*
 * startup.S  —  Arc32 RISC-V (RV32IM) Startup Code
 *
 * Responsibilities:
 *   1. Set up stack pointer
 *   2. Zero .bss section
 *   3. Copy .data section from Flash to SRAM (if needed)
 *   4. Install trap handler (mtvec)
 *   5. Call C library init (constructors for C++ globals)
 *   6. Jump to main()
 *   7. Trap handler dispatches PLIC-claimed IRQ to the right ISR
 */

    .section .init
    .global _start
    .align 2

_start:
    /* ---- Set up global pointer (for RISC-V relaxation) ---- */
    .option push
    .option norelax
    la      gp, __global_pointer$
    .option pop

    /* ---- Set up stack pointer ---- */
    la      sp, _stack_top

    /* ---- Clear .bss section ---- */
    la      t0, _bss_start
    la      t1, _bss_end
clear_bss:
    bge     t0, t1, bss_done
    sw      zero, 0(t0)
    addi    t0, t0, 4
    j       clear_bss
bss_done:

    /* ---- Copy .data section from Flash (LMA) to SRAM (VMA) ---- */
    la      t0, _data_lma_start
    la      t1, _data_start
    la      t2, _data_end
copy_data:
    bge     t1, t2, data_done
    lw      t3, 0(t0)
    sw      t3, 0(t1)
    addi    t0, t0, 4
    addi    t1, t1, 4
    j       copy_data
data_done:

    /* ---- Install trap handler ---- */
    la      t0, trap_handler
    csrw    mtvec, t0

    /* ---- Disable all interrupts until init() enables them ---- */
    csrw    mie, zero
    csrw    mstatus, zero

    /* ---- Call C runtime init then main() ---- */
    call    main

    /* main() never returns, but just in case: infinite loop */
hang:
    j       hang

/* -------------------------------------------------------------------------
 * Trap handler — handles both interrupts and exceptions.
 * For RV32IM without compressed extension, all instructions are 4 bytes,
 * so mepc += 4 is safe for synchronous exceptions we choose to skip.
 * ------------------------------------------------------------------------- */
    .section .text
    .align 2
    .global trap_handler
trap_handler:
    /* Save all caller-saved + callee-saved registers (simple, not optimal) */
    addi    sp, sp, -128
    sw      ra,   0(sp)
    sw      t0,   4(sp)
    sw      t1,   8(sp)
    sw      t2,  12(sp)
    sw      t3,  16(sp)
    sw      t4,  20(sp)
    sw      t5,  24(sp)
    sw      t6,  28(sp)
    sw      a0,  32(sp)
    sw      a1,  36(sp)
    sw      a2,  40(sp)
    sw      a3,  44(sp)
    sw      a4,  48(sp)
    sw      a5,  52(sp)
    sw      a6,  56(sp)
    sw      a7,  60(sp)
    sw      s0,  64(sp)
    sw      s1,  68(sp)

    /* Read cause */
    csrr    t0, mcause
    li      t1, 0x80000000        /* MSB set => interrupt, else exception */
    and     t2, t0, t1
    beqz    t2, exception_path

    /* ---- Interrupt path ----
     * Clear the MSB (interrupt flag) of mcause using shifts, since
     * andi only accepts a 12-bit signed immediate (0x7FFFFFFF won't fit). */
    slli    t0, t0, 1
    srli    t0, t0, 1             /* t0 = mcause with interrupt bit cleared */
    li      t1, 11                /* 11 = Machine External Interrupt (PLIC) */
    beq     t0, t1, handle_external
    li      t1, 7                 /* 7 = Machine Timer Interrupt (unused, CLINT N/A) */
    beq     t0, t1, handle_mtimer
    j       trap_restore           /* unknown interrupt source, ignore       */

handle_external:
    call    _plic_dispatch
    j       trap_restore

handle_mtimer:
    /* Not used in v1.0 (TIMER0 routed through PLIC instead) */
    j       trap_restore

exception_path:
    /* Unhandled exception: for safety, just skip the faulting instruction.
       (A future version can log mcause/mepc via UART here.) */
    csrr    t3, mepc
    addi    t3, t3, 4
    csrw    mepc, t3
    j       trap_restore

trap_restore:
    lw      ra,   0(sp)
    lw      t0,   4(sp)
    lw      t1,   8(sp)
    lw      t2,  12(sp)
    lw      t3,  16(sp)
    lw      t4,  20(sp)
    lw      t5,  24(sp)
    lw      t6,  28(sp)
    lw      a0,  32(sp)
    lw      a1,  36(sp)
    lw      a2,  40(sp)
    lw      a3,  44(sp)
    lw      a4,  48(sp)
    lw      a5,  52(sp)
    lw      a6,  56(sp)
    lw      a7,  60(sp)
    lw      s0,  64(sp)
    lw      s1,  68(sp)
    addi    sp, sp, 128
    mret
