/*
 * wiring_digital.c  —  Arc32 Digital I/O
 *
 * Implements: pinMode, digitalWrite, digitalRead,
 *             shiftOut, shiftIn, pulseIn,
 *             attachInterrupt, detachInterrupt
 */

#include "Arduino.h"

/* -----------------------------------------------------------------------
 * Internal ISR dispatch table (12 GPIO0 interrupt-capable pins)
 * ----------------------------------------------------------------------- */
#define MAX_GPIO_IRQ    12
static voidFuncPtr _gpio_isr[MAX_GPIO_IRQ] = { NULL };

/* -----------------------------------------------------------------------
 * pinMode(pin, mode)
 *
 * mode: OUTPUT → set DDR bit high (drive pin)
 *       INPUT  → clear DDR bit   (high-Z input)
 *       INPUT_PULLUP → same as INPUT (no internal pull-up on THEJAS32;
 *                      requires an external 10 K resistor to 3V3)
 * ----------------------------------------------------------------------- */
void pinMode(uint8_t pin, uint8_t mode)
{
    volatile uint32_t *dr, *ddr, *ext;
    uint8_t bit;

    if (_pin_to_gpio(pin, &dr, &ddr, &ext, &bit) != 0) {
        /* PWM pins are output-only by hardware – nothing to set */
        return;
    }

    if (mode == OUTPUT) {
        *ddr |=  (1U << bit);   /* direction → output */
    } else {
        *ddr &= ~(1U << bit);   /* direction → input  */
    }
}

/* -----------------------------------------------------------------------
 * digitalWrite(pin, val)
 * ----------------------------------------------------------------------- */
void digitalWrite(uint8_t pin, uint8_t val)
{
    volatile uint32_t *dr, *ddr, *ext;
    uint8_t bit;

    if (_pin_to_gpio(pin, &dr, &ddr, &ext, &bit) != 0) return;

    if (val) {
        *dr |=  (1U << bit);
    } else {
        *dr &= ~(1U << bit);
    }
}

/* -----------------------------------------------------------------------
 * digitalRead(pin)
 * Returns HIGH (1) or LOW (0).
 * Reads EXT register (actual pin state) regardless of direction.
 * ----------------------------------------------------------------------- */
int digitalRead(uint8_t pin)
{
    volatile uint32_t *dr, *ddr, *ext;
    uint8_t bit;

    if (_pin_to_gpio(pin, &dr, &ddr, &ext, &bit) != 0) return LOW;

    return (*ext >> bit) & 1U;
}

/* -----------------------------------------------------------------------
 * shiftOut(dataPin, clockPin, bitOrder, val)
 * ----------------------------------------------------------------------- */
void shiftOut(uint8_t dataPin, uint8_t clkPin, uint8_t bitOrder, uint8_t val)
{
    for (uint8_t i = 0; i < 8; i++) {
        uint8_t b = (bitOrder == MSBFIRST) ? (7 - i) : i;
        digitalWrite(dataPin, (val >> b) & 1);
        digitalWrite(clkPin, HIGH);
        digitalWrite(clkPin, LOW);
    }
}

/* -----------------------------------------------------------------------
 * shiftIn(dataPin, clockPin, bitOrder) → uint8_t
 * ----------------------------------------------------------------------- */
uint8_t shiftIn(uint8_t dataPin, uint8_t clkPin, uint8_t bitOrder)
{
    uint8_t val = 0;
    for (uint8_t i = 0; i < 8; i++) {
        digitalWrite(clkPin, HIGH);
        uint8_t bit = digitalRead(dataPin) ? 1 : 0;
        uint8_t pos = (bitOrder == MSBFIRST) ? (7 - i) : i;
        val |= (bit << pos);
        digitalWrite(clkPin, LOW);
    }
    return val;
}

/* -----------------------------------------------------------------------
 * pulseIn(pin, state, timeout_us) → pulse width in µs, 0 if timeout
 * ----------------------------------------------------------------------- */
uint32_t pulseIn(uint8_t pin, uint8_t state, uint32_t timeout_us)
{
    const uint32_t cyc_per_us = F_CPU / 1000000UL;
    uint32_t max_cycles = timeout_us * cyc_per_us;

    /* Wait for any previous pulse of this state to end */
    uint32_t start = read_cycle();
    while (digitalRead(pin) == state) {
        if ((read_cycle() - start) > max_cycles) return 0;
    }

    /* Wait for the target state to begin */
    start = read_cycle();
    while (digitalRead(pin) != state) {
        if ((read_cycle() - start) > max_cycles) return 0;
    }

    /* Measure the pulse width */
    uint32_t pulse_start = read_cycle();
    while (digitalRead(pin) == state) {
        if ((read_cycle() - pulse_start) > max_cycles) return 0;
    }

    return (read_cycle() - pulse_start) / cyc_per_us;
}

/* -----------------------------------------------------------------------
 * attachInterrupt(irq, handler, mode)
 *
 * irq  : use digitalPinToInterrupt(pin) – GPIO0[0:11] → IRQ 10-21
 * mode : RISING | FALLING | CHANGE
 *
 * Hardware: DesignWare APB GPIO supports level and edge detection.
 * CHANGE (both edges) is emulated by toggling polarity in the ISR.
 * ----------------------------------------------------------------------- */
void attachInterrupt(uint8_t irq, voidFuncPtr handler, int mode)
{
    if (irq < IRQ_GPIO0_BASE || irq > IRQ_GPIO0_BASE + MAX_GPIO_IRQ - 1)
        return;

    uint8_t bit = irq - IRQ_GPIO0_BASE;
    _gpio_isr[bit] = handler;

    /* Configure edge/level and polarity */
    if (mode == CHANGE) {
        /* Edge-triggered; start by detecting RISING */
        GPIO0_INTTYPE |=  (1U << bit);   /* edge                              */
        GPIO0_INTPOL  |=  (1U << bit);   /* rising first                      */
    } else if (mode == RISING) {
        GPIO0_INTTYPE |=  (1U << bit);
        GPIO0_INTPOL  |=  (1U << bit);
    } else { /* FALLING */
        GPIO0_INTTYPE |=  (1U << bit);
        GPIO0_INTPOL  &= ~(1U << bit);
    }

    GPIO0_INTMASK &= ~(1U << bit);   /* unmask */
    GPIO0_INTEN   |=  (1U << bit);   /* enable */

    PLIC_SET_PRIORITY(irq, 2);
    PLIC_ENABLE_IRQ(irq);
}

/* -----------------------------------------------------------------------
 * detachInterrupt(irq)
 * ----------------------------------------------------------------------- */
void detachInterrupt(uint8_t irq)
{
    if (irq < IRQ_GPIO0_BASE || irq > IRQ_GPIO0_BASE + MAX_GPIO_IRQ - 1)
        return;

    uint8_t bit = irq - IRQ_GPIO0_BASE;
    GPIO0_INTEN   &= ~(1U << bit);
    GPIO0_INTMASK |=  (1U << bit);
    PLIC_DISABLE_IRQ(irq);
    _gpio_isr[bit] = NULL;
}

/* -----------------------------------------------------------------------
 * GPIO interrupt dispatcher  (called from trap handler)
 * ----------------------------------------------------------------------- */
void _gpio0_irq_dispatch(void)
{
    uint32_t stat = GPIO0_INTSTAT;
    for (uint8_t i = 0; i < MAX_GPIO_IRQ; i++) {
        if ((stat >> i) & 1U) {
            GPIO0_EOI = (1U << i);           /* clear interrupt               */
            if (_gpio_isr[i]) _gpio_isr[i]();
        }
    }
}
