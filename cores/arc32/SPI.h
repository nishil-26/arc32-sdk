/*
 * SPI.h  —  Arc32 SPI Library
 */
#ifndef SPI_H
#define SPI_H

#include "Arduino.h"

#define SPI_MODE0   0x00   /* CPOL=0, CPHA=0 */
#define SPI_MODE1   0x01   /* CPOL=0, CPHA=1 */
#define SPI_MODE2   0x02   /* CPOL=1, CPHA=0 */
#define SPI_MODE3   0x03   /* CPOL=1, CPHA=1 */

class SPISettings {
public:
    SPISettings(uint32_t clock, uint8_t bitOrder, uint8_t dataMode);
    SPISettings();
    uint32_t _clock;
    uint8_t  _bitOrder;
    uint8_t  _dataMode;
};

class SPIClass {
public:
    explicit SPIClass(uint32_t base);
    void    begin(void);
    void    end(void);
    void    beginTransaction(SPISettings settings);
    void    endTransaction(void);
    uint8_t  transfer(uint8_t data);
    uint16_t transfer16(uint16_t data);
    void     transfer(void *buf, size_t count);

private:
    uint32_t _base;
    bool     _inited;
    bool     _lsb_first;
};

extern SPIClass SPI;

#endif /* SPI_H */
