/*
 * pins_arduino.h  —  Arc32 Pin Mapping
 *
 * Maps Arduino logical pin numbers to THEJAS32 GPIO banks / PWM channels.
 *
 * Physical header layout 
 *
 *   J2 LEFT HEADER          J3 RIGHT HEADER
 *   ─────────────           ───────────────
 *   1  GND                  1  GND
 *   2  D0  (GPIO0)          2  D16 (GPIO18)
 *   3  D1  (GPIO1)          3  D17 (GPIO20)
 *   4  D2  (GPIO2)          4  D18 (GPIO21)
 *   5  D3  (GPIO3)          5  D19 (GPIO22/LED)
 *   6  D4  (GPIO4)          6  D20 (GPIO23)
 *   7  D5  (GPIO5)          7  D21 (GPIO24)
 *   8  D6  (GPIO6)          8  D22 (PWM0)
 *   9  D7  (GPIO7)          9  D23 (PWM1)
 *   10 D8  (GPIO8)          10 D24 (PWM2)
 *   11 D9  (GPIO9)          11 D25 (PWM3)
 *   12 D10 (GPIO10)         12 D26 (PWM4)
 *   13 D11 (GPIO11)         13 D27 (PWM5)
 *   14 D12 (GPIO12)         14 D28 (PWM6)
 *   15 D13 (GPIO13)         15 D29 (PWM7)
 *   16 D14 (GPIO14)         16 TX1 (UART1_TX)
 *   17 D15 (GPIO15)         17 RX1 (UART1_RX)
 *   18 3V3                  18 SCL (I2C0_SCL)
 *   19 5V                   19 SDA (I2C0_SDA)
 *   20 GND                  20 GND
 *
 *   J4 EXTRA HEADER
 *   ───────────────
 *   1  SPI0_MOSI            7  SPI2_CLK
 *   2  SPI0_MISO            8  SPI2_CS
 *   3  SPI0_CLK             9  I2C1_SCL
 *   4  SPI0_CS              10 I2C1_SDA
 *   5  SPI2_MOSI            11 I2C2_SCL
 *   6  SPI2_MISO            12 I2C2_SDA
 */

#ifndef PINS_ARDUINO_H
#define PINS_ARDUINO_H

#include "thejas32.h"

/* -----------------------------------------------------------------------
 * Total pin count
 * ----------------------------------------------------------------------- */
#define NUM_DIGITAL_PINS    30   /* D0..D29                                   */
#define NUM_ANALOG_INPUTS    0   /* No on-chip ADC in THEJAS32 (ext ADC TBD)  */

/* -----------------------------------------------------------------------
 * Pin type enumeration (for pin descriptor table)
 * ----------------------------------------------------------------------- */
typedef enum {
    PIN_TYPE_GPIO0 = 0,  /* In GPIO0 bank (GPIO0–GPIO15)                     */
    PIN_TYPE_GPIO1,      /* In GPIO1 bank (GPIO16–GPIO31)                    */
    PIN_TYPE_PWM,        /* Dedicated PWM output (PWM controller)            */
} PinType;

/* -----------------------------------------------------------------------
 * Pin descriptor: bank + bit within that bank for GPIO,
 *                 channel index for PWM
 * ----------------------------------------------------------------------- */
typedef struct {
    PinType   type;
    uint8_t   bit;       /* bit position in GPIO0/GPIO1 DR/DDR/EXT           */
                         /* or PWM channel number for PWM pins               */
} PinDesc;

/*
 * Table indexed by Arduino pin number D0..D29
 *
 * GPIO0 bank: bit N → GPIO_N  (GPIO0[0..15])
 * GPIO1 bank: bit N → GPIO_(N+16)
 *
 * GPIO1 reserved bits:
 *   bit 0 = GPIO16 (WIFI_EN  – do not use)
 *   bit 1 = GPIO17 (WIFI_RST – do not use)
 *   bits 9-15 = GPIO25-31 (baud-rate divider – do not use)
 */
static const PinDesc PIN_MAP[NUM_DIGITAL_PINS] = {
    /* D0  */ { PIN_TYPE_GPIO0,  0 },   /* GPIO0  – THEJAS32 pin 74  */
    /* D1  */ { PIN_TYPE_GPIO0,  1 },   /* GPIO1  – THEJAS32 pin 73  */
    /* D2  */ { PIN_TYPE_GPIO0,  2 },   /* GPIO2  – THEJAS32 pin 72  */
    /* D3  */ { PIN_TYPE_GPIO0,  3 },   /* GPIO3  – THEJAS32 pin 71  */
    /* D4  */ { PIN_TYPE_GPIO0,  4 },   /* GPIO4  – THEJAS32 pin 66  */
    /* D5  */ { PIN_TYPE_GPIO0,  5 },   /* GPIO5  – THEJAS32 pin 65  */
    /* D6  */ { PIN_TYPE_GPIO0,  6 },   /* GPIO6  – THEJAS32 pin 64  */
    /* D7  */ { PIN_TYPE_GPIO0,  7 },   /* GPIO7  – THEJAS32 pin 63  */
    /* D8  */ { PIN_TYPE_GPIO0,  8 },   /* GPIO8  – THEJAS32 pin 62  */
    /* D9  */ { PIN_TYPE_GPIO0,  9 },   /* GPIO9  – THEJAS32 pin 61  */
    /* D10 */ { PIN_TYPE_GPIO0, 10 },   /* GPIO10 – THEJAS32 pin 58  */
    /* D11 */ { PIN_TYPE_GPIO0, 11 },   /* GPIO11 – THEJAS32 pin 55  */
    /* D12 */ { PIN_TYPE_GPIO0, 12 },   /* GPIO12 – THEJAS32 pin 54  */
    /* D13 */ { PIN_TYPE_GPIO0, 13 },   /* GPIO13 – THEJAS32 pin 53  */
    /* D14 */ { PIN_TYPE_GPIO0, 14 },   /* GPIO14 – THEJAS32 pin 52  */
    /* D15 */ { PIN_TYPE_GPIO0, 15 },   /* GPIO15 – THEJAS32 pin 51  */
    /* D16 */ { PIN_TYPE_GPIO1,  2 },   /* GPIO18 – THEJAS32 pin 2   */
    /* D17 */ { PIN_TYPE_GPIO1,  3 },   /* GPIO19 – THEJAS32 pin 1   */
    /* D18 */ { PIN_TYPE_GPIO1,  4 },   /* GPIO20 – THEJAS32 pin 128 */
    /* D19 */ { PIN_TYPE_GPIO1,  5 },   /* GPIO21 – THEJAS32 pin 127 */
    /* D20 */ { PIN_TYPE_GPIO1,  6 },   /* GPIO22 – THEJAS32 pin 122 (USER_LED) */
    /* D21 */ { PIN_TYPE_GPIO1,  7 },   /* GPIO23 – THEJAS32 pin 121 */
    /* D22 */ { PIN_TYPE_GPIO1,  8 },   /* GPIO24 – THEJAS32 pin 120 */
    /* D23 */ { PIN_TYPE_PWM,    0 },   /* PWM0   – THEJAS32 pin 86  */
    /* D24 */ { PIN_TYPE_PWM,    1 },   /* PWM1   – THEJAS32 pin 85  */
    /* D25 */ { PIN_TYPE_PWM,    2 },   /* PWM2   – THEJAS32 pin 82  */
    /* D26 */ { PIN_TYPE_PWM,    3 },   /* PWM3   – THEJAS32 pin 81  */
    /* D27 */ { PIN_TYPE_PWM,    4 },   /* PWM4   – THEJAS32 pin 80  */
    /* D28 */ { PIN_TYPE_PWM,    5 },   /* PWM5   – THEJAS32 pin 77  */
    /* D29 */ { PIN_TYPE_PWM,    6 },   /* PWM6   – THEJAS32 pin 76  */
    /* PWM7 (THEJAS32 pin 75) not assigned; use analogWrite(30,...) via extend */
};

/* -----------------------------------------------------------------------
 * Named special-purpose pins
 * ----------------------------------------------------------------------- */
#define LED_BUILTIN         20   /* D20 = GPIO22, blue user LED               */
#define LED_BUILTIN_PIN     LED_BUILTIN

/* UART pin labels (informational – not GPIO, set by hardware routing) */
#define PIN_SERIAL_TX       -1   /* UART0 via CP2102N – no GPIO number        */
#define PIN_SERIAL_RX       -1
#define PIN_SERIAL1_TX      45   /* UART1 TX – THEJAS32 pin 45 (J3 header)   */
#define PIN_SERIAL1_RX      50   /* UART1 RX – THEJAS32 pin 50               */

/* SPI0 pins (J4 header) */
#define PIN_SPI_MOSI        (-1) /* Physical: THEJAS32 pin 87, not GPIO-numbered */
#define PIN_SPI_MISO        (-1) /* Physical: THEJAS32 pin 90                    */
#define PIN_SPI_SCK         (-1) /* Physical: THEJAS32 pin 91                    */
#define PIN_SPI_SS          (-1) /* Physical: THEJAS32 pin 92                    */
#define SS                  (-1)
#define MOSI                PIN_SPI_MOSI
#define MISO                PIN_SPI_MISO
#define SCK                 PIN_SPI_SCK

/* I2C0 pins (J3 header) */
#define PIN_WIRE_SCL        (-1) /* Physical: THEJAS32 pin 33                 */
#define PIN_WIRE_SDA        (-1) /* Physical: THEJAS32 pin 34                 */
#define SDA                 PIN_WIRE_SDA
#define SCL                 PIN_WIRE_SCL

/* WiFi module control (reserved – managed by WiFi library) */
#define WIFI_EN_GPIO1_BIT   0    /* GPIO1 bit 0 = GPIO16                      */
#define WIFI_RST_GPIO1_BIT  1    /* GPIO1 bit 1 = GPIO17                      */

/* -----------------------------------------------------------------------
 * Analog-write (PWM) capable pins bitmask
 * ----------------------------------------------------------------------- */
#define digitalPinHasPWM(p) ((p) >= 23 && (p) <= 29)

/* -----------------------------------------------------------------------
 * Interrupt-capable pins
 * GPIO0[0:11] have hardware interrupt support (IRQ 10-21)
 * ----------------------------------------------------------------------- */
#define digitalPinToInterrupt(p) \
    (((p) >= 0 && (p) <= 11) ? ((p) + IRQ_GPIO0_BASE) : NOT_AN_INTERRUPT)

#define NOT_AN_INTERRUPT    (-1)

/* -----------------------------------------------------------------------
 * Internal helper: get GPIO bank base address and bit for a pin number
 * Returns 0 if pin is a PWM pin (not a GPIO pin).
 * ----------------------------------------------------------------------- */
static inline int _pin_to_gpio(uint8_t pin,
                                volatile uint32_t **dr_out,
                                volatile uint32_t **ddr_out,
                                volatile uint32_t **ext_out,
                                uint8_t           *bit_out)
{
    if (pin >= NUM_DIGITAL_PINS) return -1;
    const PinDesc *d = &PIN_MAP[pin];
    if (d->type == PIN_TYPE_GPIO0) {
        *dr_out  = (volatile uint32_t *)(GPIO0_BASE + GPIO_DR_OFF);
        *ddr_out = (volatile uint32_t *)(GPIO0_BASE + GPIO_DDR_OFF);
        *ext_out = (volatile uint32_t *)(GPIO0_BASE + GPIO_EXT_OFF);
        *bit_out = d->bit;
        return 0;
    } else if (d->type == PIN_TYPE_GPIO1) {
        *dr_out  = (volatile uint32_t *)(GPIO1_BASE + GPIO_DR_OFF);
        *ddr_out = (volatile uint32_t *)(GPIO1_BASE + GPIO_DDR_OFF);
        *ext_out = (volatile uint32_t *)(GPIO1_BASE + GPIO_EXT_OFF);
        *bit_out = d->bit;
        return 0;
    }
    return -1;  /* PWM pin */
}

#endif /* PINS_ARDUINO_H */
