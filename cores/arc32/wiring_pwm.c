/*
 * wiring_pwm.c  —  Arc32 PWM / analogWrite / tone
 *
 * analogWrite(pin, value)  where value = 0..255
 *   Pins D23..D29 map to PWM0..PWM6 (dedicated hardware PWM outputs).
 *   value=0   → output always LOW  (disable channel)
 *   value=255 → output always HIGH (period=duty)
 *
 * tone(pin, freq, duration_ms)
 *   Generates a square wave (50% duty) on a PWM pin.
 */

#include "Arduino.h"

/* Track which channels are active */
static uint8_t _pwm_enabled[PWM_NUM_CHANNELS] = { 0 };

/* -----------------------------------------------------------------------
 * _pwm_init_channel(ch)
 * First-time setup for a PWM channel: continuous, left-aligned, no IRQ.
 * ----------------------------------------------------------------------- */
static void _pwm_init_channel(uint8_t ch)
{
    /* Disable channel while configuring */
    PWM_CH_REG(ch, PWM_CTRL_OFF) = 0;
    /* Default period: PWM_DEFAULT_FREQ_HZ (1 kHz) */
    PWM_CH_REG(ch, PWM_PERIOD_OFF) = (uint32_t)PWM_PERIOD_CYCLES;
    PWM_CH_REG(ch, PWM_DUTY_OFF)   = 0;
    _pwm_enabled[ch] = 1;
}

/* -----------------------------------------------------------------------
 * analogWrite(pin, value)
 * ----------------------------------------------------------------------- */
void analogWrite(uint8_t pin, int value)
{
    if (pin < NUM_DIGITAL_PINS &&
        PIN_MAP[pin].type == PIN_TYPE_PWM)
    {
        uint8_t ch = PIN_MAP[pin].bit;   /* PWM channel index 0-6            */

        if (!_pwm_enabled[ch]) _pwm_init_channel(ch);

        if (value <= 0) {
            /* Turn off: disable channel, period and duty = 0 */
            PWM_CH_REG(ch, PWM_CTRL_OFF)   = 0;
            PWM_CH_REG(ch, PWM_DUTY_OFF)   = 0;
            return;
        }

        uint32_t period = PWM_PERIOD_CYCLES;
        uint32_t duty;

        if (value >= 255) {
            duty = period;   /* Always high */
        } else {
            duty = ((uint32_t)value * period) / 255UL;
        }

        PWM_CH_REG(ch, PWM_PERIOD_OFF) = period;
        PWM_CH_REG(ch, PWM_DUTY_OFF)   = duty;
        PWM_CH_REG(ch, PWM_CTRL_OFF)   = PWM_CTRL_ENABLE;  /* continuous     */
        return;
    }

    /* Fallback for GPIO pins: output HIGH if value > 127, else LOW */
    digitalWrite(pin, value > 127 ? HIGH : LOW);
}

/* -----------------------------------------------------------------------
 * analogWriteFrequency(pin, freq_hz)
 * Allows changing the PWM frequency per-channel.
 * ----------------------------------------------------------------------- */
void analogWriteFrequency(uint8_t pin, uint32_t freq_hz)
{
    if (pin >= NUM_DIGITAL_PINS || PIN_MAP[pin].type != PIN_TYPE_PWM) return;
    if (freq_hz == 0) return;

    uint8_t ch = PIN_MAP[pin].bit;
    uint32_t period = F_CPU / freq_hz;

    /* Preserve current duty ratio when changing frequency */
    uint32_t old_period = PWM_CH_REG(ch, PWM_PERIOD_OFF);
    uint32_t old_duty   = PWM_CH_REG(ch, PWM_DUTY_OFF);
    uint32_t new_duty   = (old_period > 0) ?
                          ((uint64_t)old_duty * period / old_period) : 0;

    PWM_CH_REG(ch, PWM_CTRL_OFF)   = 0;         /* disable while updating    */
    PWM_CH_REG(ch, PWM_PERIOD_OFF) = period;
    PWM_CH_REG(ch, PWM_DUTY_OFF)   = new_duty;
    PWM_CH_REG(ch, PWM_CTRL_OFF)   = PWM_CTRL_ENABLE;
}

/* -----------------------------------------------------------------------
 * tone(pin, frequency, duration_ms)
 * 50% duty square wave.  duration_ms = 0 → continuous until noTone().
 * ----------------------------------------------------------------------- */
void tone(uint8_t pin, uint32_t frequency, uint32_t duration_ms)
{
    if (pin >= NUM_DIGITAL_PINS || PIN_MAP[pin].type != PIN_TYPE_PWM) return;
    if (frequency == 0) { noTone(pin); return; }

    uint8_t ch = PIN_MAP[pin].bit;
    if (!_pwm_enabled[ch]) _pwm_init_channel(ch);

    uint32_t period = F_CPU / frequency;
    uint32_t duty   = period / 2;   /* 50% */

    PWM_CH_REG(ch, PWM_CTRL_OFF)   = 0;
    PWM_CH_REG(ch, PWM_PERIOD_OFF) = period;
    PWM_CH_REG(ch, PWM_DUTY_OFF)   = duty;
    PWM_CH_REG(ch, PWM_CTRL_OFF)   = PWM_CTRL_ENABLE;

    if (duration_ms > 0) {
        delay(duration_ms);
        noTone(pin);
    }
}

/* -----------------------------------------------------------------------
 * noTone(pin)
 * ----------------------------------------------------------------------- */
void noTone(uint8_t pin)
{
    if (pin >= NUM_DIGITAL_PINS || PIN_MAP[pin].type != PIN_TYPE_PWM) return;
    uint8_t ch = PIN_MAP[pin].bit;
    PWM_CH_REG(ch, PWM_CTRL_OFF) = 0;
    PWM_CH_REG(ch, PWM_DUTY_OFF) = 0;
}
