/*
 * SPI.cpp  —  Arc32 SPI Library  (SPI0, DesignWare DW_apb_ssi)
 *
 * Default: SPI0 (J4 header — MOSI pin87, MISO pin90, CLK pin91, CS pin92)
 * Clock:   up to 25 MHz (f_cpu/4)
 * Mode:    CPOL/CPHA configurable (SPI mode 0,1,2,3)
 * Frame:   8-bit, MSB first
 */

#include "SPI.h"

/* -----------------------------------------------------------------------
 * SPISettings
 * ----------------------------------------------------------------------- */
SPISettings::SPISettings(uint32_t clock, uint8_t bitOrder, uint8_t dataMode)
    : _clock(clock), _bitOrder(bitOrder), _dataMode(dataMode) {}

SPISettings::SPISettings()
    : _clock(4000000), _bitOrder(MSBFIRST), _dataMode(SPI_MODE0) {}

/* -----------------------------------------------------------------------
 * SPIClass
 * ----------------------------------------------------------------------- */
SPIClass::SPIClass(uint32_t base) : _base(base), _inited(false) {}

/* -----------------------------------------------------------------------
 * _spi_disable / _spi_enable
 * ----------------------------------------------------------------------- */
static inline void _ssi_disable(uint32_t base) {
    SPI_REG(base, SPI_SSIENR_OFF) = 0;
}
static inline void _ssi_enable(uint32_t base) {
    SPI_REG(base, SPI_SSIENR_OFF) = 1;
}

/* -----------------------------------------------------------------------
 * begin()
 * ----------------------------------------------------------------------- */
void SPIClass::begin(void)
{
    _ssi_disable(_base);

    /*
     * CTRLR0: 8-bit frame, Motorola SPI, TX+RX, CPOL=0, CPHA=0 (mode 0)
     * DFS=7 (8-bit = 7+1), FRF=0 (Motorola), SCPOL=0, SCPH=0, TMOD=0
     */
    SPI_REG(_base, SPI_CTRLR0_OFF) =
        SPI_CTRLR0_DFS(7) | SPI_CTRLR0_FRF_MOTO | SPI_CTRLR0_TMOD_TR;

    /* Default 4 MHz baud */
    SPI_REG(_base, SPI_BAUDR_OFF) = SPI_BAUDR_VAL(4000000);

    /* TX/RX thresholds */
    SPI_REG(_base, SPI_TXFTLR_OFF) = 0;
    SPI_REG(_base, SPI_RXFTLR_OFF) = 0;

    /* Mask all interrupts */
    SPI_REG(_base, SPI_IMR_OFF) = 0;

    /* Select slave 0 (CS line driven low when enabled) */
    SPI_REG(_base, SPI_SER_OFF) = 1;

    _ssi_enable(_base);
    _inited = true;
}

/* -----------------------------------------------------------------------
 * end()
 * ----------------------------------------------------------------------- */
void SPIClass::end(void)
{
    _ssi_disable(_base);
    SPI_REG(_base, SPI_SER_OFF) = 0;
    _inited = false;
}

/* -----------------------------------------------------------------------
 * beginTransaction(settings)
 * Reconfigures clock, bit order, and mode.
 * ----------------------------------------------------------------------- */
void SPIClass::beginTransaction(SPISettings settings)
{
    _ssi_disable(_base);

    uint32_t ctrlr0 = SPI_CTRLR0_DFS(7) | SPI_CTRLR0_FRF_MOTO | SPI_CTRLR0_TMOD_TR;

    /* Data mode: CPOL and CPHA */
    if (settings._dataMode == SPI_MODE1 || settings._dataMode == SPI_MODE3)
        ctrlr0 |= SPI_CTRLR0_SCPH;
    if (settings._dataMode == SPI_MODE2 || settings._dataMode == SPI_MODE3)
        ctrlr0 |= SPI_CTRLR0_SCPOL;

    /* Bit order: DW SSI always sends MSB first in hardware.
       For LSB-first, we byte-reverse in software (done in transfer). */
    _lsb_first = (settings._bitOrder == LSBFIRST);

    SPI_REG(_base, SPI_CTRLR0_OFF) = ctrlr0;

    /* Clock: BAUDR must be even, minimum 2 */
    uint32_t baudr = F_CPU / settings._clock;
    if (baudr < 2) baudr = 2;
    baudr &= ~1U;   /* round down to even */
    SPI_REG(_base, SPI_BAUDR_OFF) = baudr;

    _ssi_enable(_base);
}

/* -----------------------------------------------------------------------
 * endTransaction()
 * ----------------------------------------------------------------------- */
void SPIClass::endTransaction(void)
{
    /* Wait for SSI to finish any pending transfer */
    while (SPI_REG(_base, SPI_SR_OFF) & SPI_SR_BUSY);
}

/* -----------------------------------------------------------------------
 * transfer(data) → received byte
 * Full-duplex 8-bit transfer.
 * ----------------------------------------------------------------------- */
static uint8_t _reverse_byte(uint8_t b) {
    b = (b & 0xF0) >> 4 | (b & 0x0F) << 4;
    b = (b & 0xCC) >> 2 | (b & 0x33) << 2;
    b = (b & 0xAA) >> 1 | (b & 0x55) << 1;
    return b;
}

uint8_t SPIClass::transfer(uint8_t data)
{
    if (_lsb_first) data = _reverse_byte(data);

    /* Wait for TX FIFO not full */
    while (!(SPI_REG(_base, SPI_SR_OFF) & SPI_SR_TFNF));
    SPI_REG(_base, SPI_DR_OFF) = data;

    /* Wait for RX FIFO to have data */
    while (!(SPI_REG(_base, SPI_SR_OFF) & SPI_SR_RFNE));
    uint8_t rx = (uint8_t)SPI_REG(_base, SPI_DR_OFF);

    if (_lsb_first) rx = _reverse_byte(rx);
    return rx;
}

uint16_t SPIClass::transfer16(uint16_t data)
{
    uint8_t hi = transfer((data >> 8) & 0xFF);
    uint8_t lo = transfer(data & 0xFF);
    return ((uint16_t)hi << 8) | lo;
}

void SPIClass::transfer(void *buf, size_t count)
{
    uint8_t *b = (uint8_t *)buf;
    for (size_t i = 0; i < count; i++) {
        b[i] = transfer(b[i]);
    }
}

/* -----------------------------------------------------------------------
 * Global SPI object (SPI0)
 * ----------------------------------------------------------------------- */
SPIClass SPI(SPI0_BASE);
