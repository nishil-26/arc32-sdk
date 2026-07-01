/*
 * Wire.cpp  —  Arc32 I2C Library  (I2C0, DesignWare DW_apb_i2c, Master mode)
 *
 * Default: I2C0 (J3 header — SCL = THEJAS32 pin 33, SDA = THEJAS32 pin 34)
 * Supports: Standard (100 kHz) and Fast (400 kHz) modes.
 * Only master mode is implemented in v1.0.
 *
 * Wire object → I2C0
 * Wire1 object → I2C1 (J4 header)
 */

#include "Wire.h"

/* -----------------------------------------------------------------------
 * Constants
 * ----------------------------------------------------------------------- */
#define WIRE_BUF_SIZE       32   /* TX/RX buffer size (bytes)                */
#define I2C_TIMEOUT_CYCLES  (F_CPU / 10)   /* ~100 ms timeout               */

/* -----------------------------------------------------------------------
 * Per-I2C state
 * ----------------------------------------------------------------------- */
struct WireState {
    uint32_t base;
    uint8_t  tx_buf[WIRE_BUF_SIZE];
    uint8_t  rx_buf[WIRE_BUF_SIZE];
    uint8_t  tx_len;
    uint8_t  rx_len;
    uint8_t  rx_idx;
    uint8_t  addr;       /* 7-bit target address */
};

static struct WireState _wi[2] = {
    { I2C0_BASE, {0},{0},0,0,0,0 },
    { I2C1_BASE, {0},{0},0,0,0,0 },
};

/* -----------------------------------------------------------------------
 * Low-level I2C helpers
 * ----------------------------------------------------------------------- */
static void _i2c_disable(uint32_t base) {
    I2C_REG(base, I2C_ENABLE_OFF) = 0;
    /* Poll until disabled */
    uint32_t timeout = I2C_TIMEOUT_CYCLES;
    while ((I2C_REG(base, I2C_ENABLE_STAT_OFF) & 0x01) && --timeout);
}

static void _i2c_enable(uint32_t base) {
    I2C_REG(base, I2C_ENABLE_OFF) = 1;
}

static void _i2c_hw_init(uint32_t base, uint32_t freq_hz)
{
    _i2c_disable(base);

    uint32_t con = I2C_CON_MASTER | I2C_CON_RESTART_EN | I2C_CON_SLAVE_DISABLE;

    if (freq_hz <= 100000) {
        con |= I2C_CON_SPEED_STD;
        I2C_REG(base, I2C_SS_SCL_HCNT_OFF) = I2C_SS_HCNT;
        I2C_REG(base, I2C_SS_SCL_LCNT_OFF) = I2C_SS_LCNT;
    } else {
        con |= I2C_CON_SPEED_FAST;
        I2C_REG(base, I2C_FS_SCL_HCNT_OFF) = I2C_FS_HCNT;
        I2C_REG(base, I2C_FS_SCL_LCNT_OFF) = I2C_FS_LCNT;
    }

    I2C_REG(base, I2C_CON_OFF) = con;
    I2C_REG(base, I2C_INTR_MASK_OFF) = 0;   /* Mask all interrupts (polling) */
    I2C_REG(base, I2C_RX_TL_OFF) = 0;       /* RX FIFO threshold = 1 byte    */
    I2C_REG(base, I2C_TX_TL_OFF) = 0;       /* TX FIFO threshold = 0         */

    _i2c_enable(base);
}

static bool _i2c_wait_tx_done(uint32_t base)
{
    uint32_t t = I2C_TIMEOUT_CYCLES;
    /* Wait for TX FIFO empty AND master not busy */
    while (t--) {
        uint32_t st = I2C_REG(base, I2C_STATUS_OFF);
        if ((st & I2C_STATUS_TFE) && !(st & I2C_STATUS_MST_ACTIVITY))
            return true;
        /* Check for abort */
        if (I2C_REG(base, I2C_RAW_INTR_OFF) & I2C_RAW_TX_ABRT) {
            /* Clear abort */
            (void)I2C_REG(base, I2C_CLR_TX_ABRT_OFF);
            return false;
        }
    }
    return false;   /* timeout */
}

static bool _i2c_wait_stop(uint32_t base)
{
    uint32_t t = I2C_TIMEOUT_CYCLES;
    while (t--) {
        if (I2C_REG(base, I2C_RAW_INTR_OFF) & I2C_RAW_STOP_DET) {
            (void)I2C_REG(base, I2C_CLR_STOP_DET);
            return true;
        }
    }
    return false;
}

/* -----------------------------------------------------------------------
 * TwoWire implementation
 * ----------------------------------------------------------------------- */
TwoWire::TwoWire(uint8_t idx) : _idx(idx) {}

void TwoWire::begin(void)
{
    _i2c_hw_init(_wi[_idx].base, 100000);
}

void TwoWire::begin(uint8_t addr)
{
    (void)addr;   /* Slave mode not implemented */
    begin();
}

void TwoWire::end(void)
{
    _i2c_disable(_wi[_idx].base);
}

void TwoWire::setClock(uint32_t freq_hz)
{
    _i2c_hw_init(_wi[_idx].base, freq_hz);
}

/* -----------------------------------------------------------------------
 * beginTransmission(addr)
 * ----------------------------------------------------------------------- */
void TwoWire::beginTransmission(uint8_t addr)
{
    struct WireState *w = &_wi[_idx];
    w->addr   = addr & 0x7F;
    w->tx_len = 0;
}

/* -----------------------------------------------------------------------
 * write() — buffer bytes for transmission
 * ----------------------------------------------------------------------- */
size_t TwoWire::write(uint8_t data)
{
    struct WireState *w = &_wi[_idx];
    if (w->tx_len >= WIRE_BUF_SIZE) return 0;
    w->tx_buf[w->tx_len++] = data;
    return 1;
}

size_t TwoWire::write(const uint8_t *data, size_t len)
{
    size_t n = 0;
    for (size_t i = 0; i < len; i++) n += write(data[i]);
    return n;
}

/* -----------------------------------------------------------------------
 * endTransmission(sendStop)
 * Returns: 0=success, 1=data too long, 2=NACK on addr, 3=NACK on data, 4=other
 * ----------------------------------------------------------------------- */
uint8_t TwoWire::endTransmission(bool sendStop)
{
    struct WireState *w = &_wi[_idx];
    uint32_t base = w->base;

    if (w->tx_len == 0) return 0;

    /* Set target address */
    _i2c_disable(base);
    I2C_REG(base, I2C_TAR_OFF) = w->addr;
    _i2c_enable(base);

    /* Write bytes to TX FIFO */
    for (uint8_t i = 0; i < w->tx_len; i++) {
        uint32_t cmd = w->tx_buf[i];
        /* Last byte: optionally set STOP bit */
        if (i == w->tx_len - 1 && sendStop)
            cmd |= I2C_DATA_CMD_STOP;
        /* Wait for TX FIFO not full */
        uint32_t t = I2C_TIMEOUT_CYCLES;
        while (!(I2C_REG(base, I2C_STATUS_OFF) & I2C_STATUS_TFNF) && t--);
        I2C_REG(base, I2C_DATA_CMD_OFF) = cmd;
    }

    if (!_i2c_wait_tx_done(base)) return 3;
    if (sendStop && !_i2c_wait_stop(base)) return 4;

    /* Check TX abort source for NACK */
    uint32_t abrt = I2C_REG(base, I2C_TX_ABRT_SRC_OFF);
    if (abrt & 0x01) return 2;   /* ABRT_7B_ADDR_NOACK */
    if (abrt & 0x08) return 3;   /* ABRT_TXDATA_NOACK  */

    w->tx_len = 0;
    return 0;
}

uint8_t TwoWire::endTransmission(void)
{
    return endTransmission(true);
}

/* -----------------------------------------------------------------------
 * requestFrom(addr, count, sendStop)
 * Returns: number of bytes actually read
 * ----------------------------------------------------------------------- */
uint8_t TwoWire::requestFrom(uint8_t addr, uint8_t count, bool sendStop)
{
    struct WireState *w = &_wi[_idx];
    uint32_t base = w->base;

    if (count == 0 || count > WIRE_BUF_SIZE) return 0;

    /* Set target address */
    _i2c_disable(base);
    I2C_REG(base, I2C_TAR_OFF) = addr & 0x7F;
    _i2c_enable(base);

    /* Issue read commands */
    for (uint8_t i = 0; i < count; i++) {
        uint32_t cmd = I2C_DATA_CMD_READ;
        if (i == count - 1 && sendStop) cmd |= I2C_DATA_CMD_STOP;
        /* Wait for TX FIFO space */
        uint32_t t = I2C_TIMEOUT_CYCLES;
        while (!(I2C_REG(base, I2C_STATUS_OFF) & I2C_STATUS_TFNF) && t--);
        I2C_REG(base, I2C_DATA_CMD_OFF) = cmd;
    }

    /* Read received bytes */
    w->rx_len = 0;
    w->rx_idx = 0;
    for (uint8_t i = 0; i < count; i++) {
        uint32_t t = I2C_TIMEOUT_CYCLES;
        while (!(I2C_REG(base, I2C_STATUS_OFF) & I2C_STATUS_RFNE) && t--);
        if (!t) break;
        w->rx_buf[w->rx_len++] = (uint8_t)I2C_REG(base, I2C_DATA_CMD_OFF);
    }

    if (sendStop) _i2c_wait_stop(base);
    return w->rx_len;
}

uint8_t TwoWire::requestFrom(uint8_t addr, uint8_t count)
{
    return requestFrom(addr, count, true);
}

int TwoWire::available(void)
{
    struct WireState *w = &_wi[_idx];
    return w->rx_len - w->rx_idx;
}

int TwoWire::read(void)
{
    struct WireState *w = &_wi[_idx];
    if (w->rx_idx >= w->rx_len) return -1;
    return w->rx_buf[w->rx_idx++];
}

int TwoWire::peek(void)
{
    struct WireState *w = &_wi[_idx];
    if (w->rx_idx >= w->rx_len) return -1;
    return w->rx_buf[w->rx_idx];
}

void TwoWire::flush(void) { /* Nothing to flush in polling mode */ }

size_t TwoWire::print(const char *s)
{
    size_t n = 0;
    while (*s) n += write((uint8_t)*s++);
    return n;
}

/* -----------------------------------------------------------------------
 * Global Wire objects
 * ----------------------------------------------------------------------- */
TwoWire Wire(0);    /* I2C0 – J3 header */
TwoWire Wire1(1);   /* I2C1 – J4 header */
