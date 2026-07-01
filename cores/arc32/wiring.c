/*
 * wiring.c  —  Arc32 Timing Functions
 *
 * millis()  : TIMER0 in periodic 1-ms mode, ISR increments counter
 * micros()  : TIMER1 free-running, microseconds from cycle counter
 * delay()   : millis()-based spin
 * delayMicroseconds() : busy-wait on RISC-V rdcycle CSR
 */

#include "Arduino.h"

/* -----------------------------------------------------------------------
 * Global state
 * ----------------------------------------------------------------------- */
volatile uint32_t _arc32_millis_count = 0;   /* incremented by TIMER0 ISR   */
uint32_t          _arc32_rng_state    = 1;   /* LCG seed                    */

/* -----------------------------------------------------------------------
 * TIMER0 ISR  (called from trap handler in startup.S)
 * ----------------------------------------------------------------------- */
void __attribute__((interrupt)) _timer0_isr(void)
{
    /* Reading EOI register clears the interrupt */
    (void)TIMER_REG(TIMER0_BASE, TIMER_EOI_OFF);
    _arc32_millis_count++;
}

/* -----------------------------------------------------------------------
 * init()  —  called once by main() before setup()
 * Configures TIMER0 for 1-ms tick and enables machine external interrupts.
 * ----------------------------------------------------------------------- */
void init(void)
{
    /* ── TIMER0: 1 ms periodic ──────────────────────────────────── */
    /* Disable first */
    TIMER_REG(TIMER0_BASE, TIMER_CONTROLREG_OFF) = 0;

    /* Load count: 99999 → counts down to 0 every 1 ms @ 100 MHz */
    TIMER_REG(TIMER0_BASE, TIMER_LOADCOUNT_OFF) = TIMER_1MS_LOAD;

    /* Enable: user-defined count mode (periodic), interrupt unmasked */
    TIMER_REG(TIMER0_BASE, TIMER_CONTROLREG_OFF) =
        TIMER_CTRL_ENABLE | TIMER_CTRL_MODE_USER;

    /* Clear any pending interrupt */
    (void)TIMER_REG(TIMER0_BASE, TIMER_EOI_OFF);

    /* ── PLIC: enable TIMER0 interrupt (IRQ 7) ──────────────────── */
    PLIC_SET_PRIORITY(IRQ_TIMER0, 1);
    PLIC_ENABLE_IRQ(IRQ_TIMER0);
    PLIC_SET_THRESHOLD(0);   /* Accept all priorities ≥ 1             */

    /* ── RISC-V: enable machine external interrupts ─────────────── */
    csr_set(mie, MIE_MEIE);
    csr_set(mstatus, MSTATUS_MIE);
}

/* -----------------------------------------------------------------------
 * millis()
 * ----------------------------------------------------------------------- */
uint32_t millis(void)
{
    uint32_t v;
    disable_irq();
    v = _arc32_millis_count;
    enable_irq();
    return v;
}

/* -----------------------------------------------------------------------
 * micros()
 * Uses RISC-V rdcycle: 1 cycle = 10 ns @ 100 MHz → 1 µs = 100 cycles.
 * Wraps approximately every 42.9 seconds (32-bit, 100 MHz).
 * ----------------------------------------------------------------------- */
uint32_t micros(void)
{
    return read_cycle() / (F_CPU / 1000000UL);
}

/* -----------------------------------------------------------------------
 * delay(ms)
 * ----------------------------------------------------------------------- */
void delay(uint32_t ms)
{
    uint32_t start = millis();
    while ((millis() - start) < ms) {
        yield();
    }
}

/* -----------------------------------------------------------------------
 * delayMicroseconds(us)
 * Busy-wait on rdcycle.  Accurate to ± 1 µs.
 * For very short delays (us == 0 or us == 1) returns immediately.
 * ----------------------------------------------------------------------- */
void delayMicroseconds(uint32_t us)
{
    if (us == 0) return;
    const uint32_t cycles_per_us = F_CPU / 1000000UL;   /* = 100          */
    busy_wait_cycles(us * cycles_per_us);
}

/* -----------------------------------------------------------------------
 * yield()  —  hook for cooperative multitasking (stub)
 * ----------------------------------------------------------------------- */
void yield(void) { /* nothing in v1.0 */ }
