/*
 * Arduino.h  —  Arc32 Arduino Core
 * Top-level header: include this in every sketch (done automatically by IDE)
 */

#ifndef ARDUINO_H
#define ARDUINO_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include <math.h>

#include "thejas32.h"
#include "binary.h"

#ifdef __cplusplus
extern "C" {
#endif

/* -----------------------------------------------------------------------
 * Constants (Arduino standard)
 * ----------------------------------------------------------------------- */
#define HIGH        1
#define LOW         0
#define INPUT       0
#define OUTPUT      1
#define INPUT_PULLUP 2   /* THEJAS32 GPIO has no internal pull-up;
                            treated as INPUT — add external 10K pull-up   */

#define true        1
#define false       0

#define PI          3.14159265358979323846
#define HALF_PI     1.5707963267948966
#define TWO_PI      6.283185307179586
#define DEG_TO_RAD  0.017453292519943295
#define RAD_TO_DEG  57.29577951308232
#define EULER       2.718281828459045

#define LSBFIRST    0
#define MSBFIRST    1

#define CHANGE      3
#define FALLING     2
#define RISING      1

/* -----------------------------------------------------------------------
 * Type aliases
 * ----------------------------------------------------------------------- */
typedef bool     boolean;
typedef uint8_t  byte;
typedef uint16_t word;

/* -----------------------------------------------------------------------
 * Timing  (wiring.c)
 * ----------------------------------------------------------------------- */
void     init(void);                     /* Called before setup() by main()  */
uint32_t millis(void);
uint32_t micros(void);
void     delay(uint32_t ms);
void     delayMicroseconds(uint32_t us);
void     yield(void);                    /* Stub – call from long loops       */

/* -----------------------------------------------------------------------
 * Digital I/O  (wiring_digital.c)
 * ----------------------------------------------------------------------- */
void    pinMode(uint8_t pin, uint8_t mode);
void    digitalWrite(uint8_t pin, uint8_t val);
int     digitalRead(uint8_t pin);

/* -----------------------------------------------------------------------
 * PWM  (wiring_pwm.c)
 * ----------------------------------------------------------------------- */
void    analogWrite(uint8_t pin, int val);

/* -----------------------------------------------------------------------
 * Interrupts
 * ----------------------------------------------------------------------- */
typedef void (*voidFuncPtr)(void);
void    attachInterrupt(uint8_t irq, voidFuncPtr handler, int mode);
void    detachInterrupt(uint8_t irq);

static inline void interrupts(void)   { enable_irq();  }
static inline void noInterrupts(void) { disable_irq(); }

/* NOTE: Math helpers (min/max/abs/constrain templates) and random()
 * are C++-only and are defined further below, after the extern "C"
 * block closes — templates and C++ overloads cannot have C linkage. */
#ifndef __cplusplus
/* Plain-C builds (rare in this core, but supported): simple macros. */
#define min(a,b)        ((a)<(b)?(a):(b))
#define max(a,b)        ((a)>(b)?(a):(b))
#define abs(x)          ((x)>0?(x):-(x))
#define constrain(x,lo,hi) ((x)<(lo)?(lo):((x)>(hi)?(hi):(x)))
#define radians(deg)    ((deg)*DEG_TO_RAD)
#define degrees(rad)    ((rad)*RAD_TO_DEG)
#define sq(x)           ((x)*(x))
#define map(val,fl,fh,tl,th) \
    ((long)(val - fl) * (th - tl) / (fh - fl) + tl)
#endif

/* -----------------------------------------------------------------------
 * Bit manipulation
 * ----------------------------------------------------------------------- */
#define bitRead(val,bit)        (((val)>>(bit))&1)
#define bitSet(val,bit)         ((val)|=(1UL<<(bit)))
#define bitClear(val,bit)       ((val)&=~(1UL<<(bit)))
#define bitToggle(val,bit)      ((val)^=(1UL<<(bit)))
#define bitWrite(val,bit,bval)  ((bval)?bitSet(val,bit):bitClear(val,bit))
#define bit(b)                  (1UL<<(b))

#define lowByte(w)              ((uint8_t)((w) & 0xFF))
#define highByte(w)             ((uint8_t)(((w)>>8) & 0xFF))

/* -----------------------------------------------------------------------
 * Pulse / shift (implemented in wiring_digital.c)
 * ----------------------------------------------------------------------- */
uint32_t pulseIn(uint8_t pin, uint8_t state, uint32_t timeout_us);
void     shiftOut(uint8_t dataPin, uint8_t clkPin, uint8_t bitOrder, uint8_t val);
uint8_t  shiftIn(uint8_t dataPin, uint8_t clkPin, uint8_t bitOrder);

/* -----------------------------------------------------------------------
 * Tone (stub in v1.0 – uses PWM under the hood)
 * ----------------------------------------------------------------------- */
void tone(uint8_t pin, uint32_t frequency, uint32_t duration_ms);
void noTone(uint8_t pin);

#ifdef __cplusplus
} /* extern "C" */

/* -----------------------------------------------------------------------
 * Math helpers (C++ only — templates/overloads cannot have C linkage,
 * so these must live outside the extern "C" block above).
 * ----------------------------------------------------------------------- */
template<typename T> static inline T min(T a, T b) { return (a) < (b) ? a : b; }
template<typename T> static inline T max(T a, T b) { return (a) > (b) ? a : b; }
template<typename T> static inline T abs(T x)      { return (x) > 0 ? x : -x; }
template<typename T> static inline T constrain(T x, T lo, T hi) {
    return (x) < (lo) ? (lo) : ((x) > (hi) ? (hi) : (x));
}

#define radians(deg)    ((deg)*DEG_TO_RAD)
#define degrees(rad)    ((rad)*RAD_TO_DEG)
#define sq(x)           ((x)*(x))
#define map(val,fl,fh,tl,th) \
    ((long)(val - fl) * (th - tl) / (fh - fl) + tl)

/* picolibc's <stdlib.h> already declares a POSIX random()/srandom()
 * with C linkage and a different signature, so we cannot reuse those
 * names directly for our own function — implement under a different
 * name and map the familiar Arduino call syntax onto it via macros. */
static inline long arc32_random(long howbig) {
    if (howbig == 0) return 0;
    extern uint32_t _arc32_rng_state;          /* simple LCG, not crypto */
    _arc32_rng_state = _arc32_rng_state * 1664525UL + 1013904223UL;
    return (long)(_arc32_rng_state % (unsigned long)howbig);
}
static inline long arc32_random_range(long lo, long hi) {
    return lo + arc32_random(hi - lo);
}
static inline void arc32_random_seed(uint32_t seed) {
    extern uint32_t _arc32_rng_state;
    _arc32_rng_state = seed;
}
#define ARC32_RANDOM_PICK(_1,_2,NAME,...) NAME
#define random(...)      ARC32_RANDOM_PICK(__VA_ARGS__, arc32_random_range, arc32_random)(__VA_ARGS__)
#define randomSeed(seed) arc32_random_seed(seed)

/* C++ includes */
#include "WString.h"
#include "Print.h"
#include "Stream.h"
#include "HardwareSerial.h"
#include "SPI.h"
#include "Wire.h"

/* Global Serial objects */
extern HardwareSerial Serial;    /* UART0 via CP2102N – USB port             */
extern HardwareSerial Serial1;   /* UART1 – J3 header pins TX1/RX1           */
extern HardwareSerial Serial2;   /* UART2 – WiFi module                      */

/* -----------------------------------------------------------------------
 * Sketch entry points (defined by user)
 * ----------------------------------------------------------------------- */
extern void setup(void);
extern void loop(void);

#endif /* __cplusplus */

/* -----------------------------------------------------------------------
 * Variant-specific pin definitions
 * ----------------------------------------------------------------------- */
#include "pins_arduino.h"

#endif /* ARDUINO_H */
