/*
 * HardwareSerial.cpp  —  Arc32 UART Driver
 *
 * Supports UART0 (Serial  – CP2102N USB), 
 *           UART1 (Serial1 – J3 header),
 *           UART2 (Serial2 – RTL8720DN WiFi).
 *
 * TX: polling (writes directly to THR when THRE=1).
 * RX: interrupt-driven with 256-byte circular ring buffer.
 */

#include "HardwareSerial.h"

/* -----------------------------------------------------------------------
 * Ring buffer
 * ----------------------------------------------------------------------- */
#define SERIAL_RX_BUF 256   /* must be power of 2 */
#define SERIAL_TX_BUF 64

struct RingBuf {
    volatile uint8_t buf[SERIAL_RX_BUF];
    volatile uint16_t head;
    volatile uint16_t tail;
};

static inline bool rb_empty(const struct RingBuf *rb) {
    return rb->head == rb->tail;
}
static inline bool rb_full(const struct RingBuf *rb) {
    return ((rb->head + 1) & (SERIAL_RX_BUF - 1)) == rb->tail;
}
static inline void rb_push(struct RingBuf *rb, uint8_t b) {
    if (!rb_full(rb)) {
        rb->buf[rb->head] = b;
        rb->head = (rb->head + 1) & (SERIAL_RX_BUF - 1);
    }
    /* if full, byte is dropped (overrun) */
}
static inline int rb_pop(struct RingBuf *rb) {
    if (rb_empty(rb)) return -1;
    uint8_t b = rb->buf[rb->tail];
    rb->tail = (rb->tail + 1) & (SERIAL_RX_BUF - 1);
    return b;
}

/* Per-UART state */
struct UARTState {
    uint32_t      base;
    struct RingBuf rx;
    uint8_t       irq;
};

static struct UARTState _uart[3] = {
    { UART0_BASE, {{0},0,0}, IRQ_UART0 },
    { UART1_BASE, {{0},0,0}, IRQ_UART1 },
    { UART2_BASE, {{0},0,0}, IRQ_UART2 },
};

/* -----------------------------------------------------------------------
 * Low-level UART helpers
 * ----------------------------------------------------------------------- */
static void _uart_set_baud(uint32_t base, uint32_t baud)
{
    uint32_t div = UART_BAUD_DIV(baud);
    /* Enable divisor latch access */
    UART_REG(base, UART_LCR_OFF) = UART_LCR_DLAB;
    UART_REG(base, UART_DLL_OFF) = div & 0xFF;
    UART_REG(base, UART_DLH_OFF) = (div >> 8) & 0xFF;
    /* 8N1, disable DLAB */
    UART_REG(base, UART_LCR_OFF) = UART_LCR_WLS_8;
}

static void _uart_hw_init(uint32_t base, uint32_t baud)
{
    _uart_set_baud(base, baud);
    /* Enable and clear FIFOs, RX trigger = 1 byte */
    UART_REG(base, UART_FCR_OFF) =
        UART_FCR_FIFOEN | UART_FCR_RXCLR | UART_FCR_TXCLR | UART_FCR_RXTRIG_1;
    /* Enable RX interrupt only */
    UART_REG(base, UART_IER_OFF) = UART_IER_RDI;
}

static inline bool _uart_tx_ready(uint32_t base) {
    return (UART_REG(base, UART_LSR_OFF) & UART_LSR_THRE) != 0;
}
static inline bool _uart_rx_ready(uint32_t base) {
    return (UART_REG(base, UART_LSR_OFF) & UART_LSR_DR) != 0;
}
static inline void _uart_tx_byte(uint32_t base, uint8_t b) {
    while (!_uart_tx_ready(base));
    UART_REG(base, UART_THR_OFF) = b;
}

/* -----------------------------------------------------------------------
 * UART RX ISR (called from trap handler per UART index)
 * ----------------------------------------------------------------------- */
static void _uart_rx_isr(uint8_t idx)
{
    struct UARTState *u = &_uart[idx];
    /* Drain the RX FIFO */
    while (_uart_rx_ready(u->base)) {
        uint8_t b = (uint8_t)UART_REG(u->base, UART_RBR_OFF);
        rb_push(&u->rx, b);
    }
}
void _uart0_isr(void) { _uart_rx_isr(0); }
void _uart1_isr(void) { _uart_rx_isr(1); }
void _uart2_isr(void) { _uart_rx_isr(2); }

/* -----------------------------------------------------------------------
 * HardwareSerial implementation
 * ----------------------------------------------------------------------- */
HardwareSerial::HardwareSerial(uint8_t uart_idx)
    : _idx(uart_idx), _baud(0) {}

void HardwareSerial::begin(uint32_t baud)
{
    _baud = baud;
    struct UARTState *u = &_uart[_idx];
    _uart_hw_init(u->base, baud);

    /* Enable IRQ in PLIC */
    PLIC_SET_PRIORITY(u->irq, 1);
    PLIC_ENABLE_IRQ(u->irq);
    csr_set(mie, MIE_MEIE);
    csr_set(mstatus, MSTATUS_MIE);
}

void HardwareSerial::begin(uint32_t baud, uint8_t config)
{
    /* config byte: low nibble = bits, upper nibble = parity/stop (future)  */
    (void)config;
    begin(baud);
}

void HardwareSerial::end(void)
{
    /* Flush TX, disable RX interrupt */
    flush();
    UART_REG(_uart[_idx].base, UART_IER_OFF) = 0;
    PLIC_DISABLE_IRQ(_uart[_idx].irq);
    _baud = 0;
}

int HardwareSerial::available(void)
{
    struct UARTState *u = &_uart[_idx];
    disable_irq();
    int n = (u->rx.head - u->rx.tail) & (SERIAL_RX_BUF - 1);
    enable_irq();
    return n;
}

int HardwareSerial::peek(void)
{
    struct UARTState *u = &_uart[_idx];
    if (rb_empty(&u->rx)) return -1;
    return u->rx.buf[u->rx.tail];
}

int HardwareSerial::read(void)
{
    disable_irq();
    int b = rb_pop(&_uart[_idx].rx);
    enable_irq();
    return b;
}

void HardwareSerial::flush(void)
{
    /* Wait until TX shift register is empty */
    uint32_t base = _uart[_idx].base;
    while (!(UART_REG(base, UART_LSR_OFF) & UART_LSR_TEMT));
}

size_t HardwareSerial::write(uint8_t b)
{
    _uart_tx_byte(_uart[_idx].base, b);
    return 1;
}

size_t HardwareSerial::write(const uint8_t *buf, size_t n)
{
    uint32_t base = _uart[_idx].base;
    for (size_t i = 0; i < n; i++) _uart_tx_byte(base, buf[i]);
    return n;
}

HardwareSerial::operator bool() { return _baud != 0; }

/* -----------------------------------------------------------------------
 * Global Serial objects
 * ----------------------------------------------------------------------- */
HardwareSerial Serial(0);    /* UART0 – USB via CP2102N */
HardwareSerial Serial1(1);   /* UART1 – J3 header       */
HardwareSerial Serial2(2);   /* UART2 – WiFi module     */
