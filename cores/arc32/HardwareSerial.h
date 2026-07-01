/*
 * HardwareSerial.h  —  Arc32 UART Class
 */
#ifndef HARDWARE_SERIAL_H
#define HARDWARE_SERIAL_H

#include <stdint.h>
#include <stddef.h>
#include "Stream.h"
#include "thejas32.h"

/* UART config byte constants (for begin(baud, config)) */
#define SERIAL_5N1  0x00
#define SERIAL_6N1  0x02
#define SERIAL_7N1  0x04
#define SERIAL_8N1  0x06   /* default */
#define SERIAL_8N2  0x0E
#define SERIAL_8E1  0x26
#define SERIAL_8O1  0x36

class HardwareSerial : public Stream {
public:
    explicit HardwareSerial(uint8_t uart_idx);

    void   begin(uint32_t baud);
    void   begin(uint32_t baud, uint8_t config);
    void   end(void);

    /* Stream interface */
    int    available(void) override;
    int    peek(void)      override;
    int    read(void)      override;
    void   flush(void)     override;

    /* Print interface */
    size_t write(uint8_t b)                    override;
    size_t write(const uint8_t *buf, size_t n) override;
    using Print::write;

    operator bool();

private:
    uint8_t  _idx;
    uint32_t _baud;
};

/* ISR forward declarations (called from trap.S) */
#ifdef __cplusplus
extern "C" {
#endif
void _uart0_isr(void);
void _uart1_isr(void);
void _uart2_isr(void);
#ifdef __cplusplus
}
#endif

extern HardwareSerial Serial;
extern HardwareSerial Serial1;
extern HardwareSerial Serial2;

#endif /* HARDWARE_SERIAL_H */
