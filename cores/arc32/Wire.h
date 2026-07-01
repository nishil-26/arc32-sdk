/*
 * Wire.h  —  Arc32 I2C Library
 */
#ifndef WIRE_H
#define WIRE_H

#include "Arduino.h"

class TwoWire {
public:
    explicit TwoWire(uint8_t idx);

    void    begin(void);
    void    begin(uint8_t addr);       /* slave mode not implemented v1.0 */
    void    end(void);
    void    setClock(uint32_t freq_hz);

    void    beginTransmission(uint8_t addr);
    size_t  write(uint8_t data);
    size_t  write(const uint8_t *data, size_t len);
    uint8_t endTransmission(bool sendStop);
    uint8_t endTransmission(void);

    uint8_t requestFrom(uint8_t addr, uint8_t count, bool sendStop);
    uint8_t requestFrom(uint8_t addr, uint8_t count);

    int     available(void);
    int     read(void);
    int     peek(void);
    void    flush(void);

    size_t  print(const char *s);   /* convenience, not standard Arduino   */

private:
    uint8_t _idx;
};

extern TwoWire Wire;     /* I2C0 */
extern TwoWire Wire1;    /* I2C1 */

#endif /* WIRE_H */
