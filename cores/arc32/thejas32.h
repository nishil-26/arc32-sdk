/*
 * thejas32.h - THEJAS32 SoC Complete Register Map
 * Arc32 Arduino SDK
 *
 * THEJAS32: RISC-V RV32IM, VEGA ET1031 core, 100 MHz, 256 KB SRAM
 * All addresses from THEJAS32 Datasheet Table 4 (Memory Map)
 * Peripheral IP: DesignWare APB GPIO / UART / SPI / I2C  +  custom PWM/TIMER
 */

#ifndef THEJAS32_H
#define THEJAS32_H

#include <stdint.h>

/* -----------------------------------------------------------------------
 * System Clock
 * ----------------------------------------------------------------------- */
#ifndef F_CPU
#define F_CPU               100000000UL   /* 100 MHz */
#endif

/* -----------------------------------------------------------------------
 * Memory Map  (Table 4 - THEJAS32 Datasheet)
 * ----------------------------------------------------------------------- */
#define SRAM_BASE           0x00200000UL
#define SRAM_SIZE           (256 * 1024)

#define UART0_BASE          0x10000100UL
#define UART1_BASE          0x10000200UL
#define UART2_BASE          0x10000300UL

#define SPI0_BASE           0x10000600UL
#define SPI1_BASE           0x10000700UL

#define I2C0_BASE           0x10000800UL
#define I2C1_BASE           0x10000900UL

#define TIMER0_BASE         0x10000A00UL
#define TIMER1_BASE         0x10000A14UL
#define TIMER2_BASE         0x10000A28UL
#define TIMERS_INTSTATUS    0x10000AA0UL
#define TIMERS_EOI          0x10000AA4UL
#define TIMERS_RAWSTATUS    0x10000AA8UL

#define I2C2_BASE           0x10001000UL

/* GPIO0 covers GPIO[0:15], GPIO1 covers GPIO[16:31]                       */
#define GPIO0_BASE          0x10080000UL
#define GPIO1_BASE          0x100C0000UL

#define SPI2_BASE           0x10200100UL

#define PWM_BASE            0x10400000UL

#define PLIC_BASE           0x20010000UL

/* -----------------------------------------------------------------------
 * Register Access Macros
 * ----------------------------------------------------------------------- */
#define REG32(addr)         (*((volatile uint32_t *)(addr)))
#define REG16(addr)         (*((volatile uint16_t *)(addr)))
#define REG8(addr)          (*((volatile uint8_t  *)(addr)))

/* -----------------------------------------------------------------------
 * GPIO  (DesignWare APB GPIO)
 * Two banks: GPIO0 (bits 15:0 = GPIO15..GPIO0)
 *            GPIO1 (bits 15:0 = GPIO31..GPIO16)
 * ----------------------------------------------------------------------- */
#define GPIO_DR_OFF         0x00   /* Data Register           (output drive) */
#define GPIO_DDR_OFF        0x04   /* Data Direction Register  1=out, 0=in   */
#define GPIO_CTL_OFF        0x08   /* Port Control                            */
#define GPIO_INTEN_OFF      0x30   /* Interrupt Enable                        */
#define GPIO_INTMASK_OFF    0x34   /* Interrupt Mask                          */
#define GPIO_INTTYPE_OFF    0x38   /* Interrupt Type (0=level, 1=edge)        */
#define GPIO_INTPOL_OFF     0x3C   /* Interrupt Polarity (0=low/fall, 1=hi/rise) */
#define GPIO_INTSTAT_OFF    0x40   /* Interrupt Status                        */
#define GPIO_RAWSTAT_OFF    0x44   /* Raw Interrupt Status                    */
#define GPIO_DEBOUNCE_OFF   0x48   /* Debounce Enable                         */
#define GPIO_EOI_OFF        0x4C   /* Clear Interrupt (write 1 to clear)      */
#define GPIO_EXT_OFF        0x50   /* External Port (read actual pin state)   */

/* Convenience macros for GPIO0 */
#define GPIO0_DR            REG32(GPIO0_BASE + GPIO_DR_OFF)
#define GPIO0_DDR           REG32(GPIO0_BASE + GPIO_DDR_OFF)
#define GPIO0_EXT           REG32(GPIO0_BASE + GPIO_EXT_OFF)
#define GPIO0_INTEN         REG32(GPIO0_BASE + GPIO_INTEN_OFF)
#define GPIO0_INTMASK       REG32(GPIO0_BASE + GPIO_INTMASK_OFF)
#define GPIO0_INTTYPE       REG32(GPIO0_BASE + GPIO_INTTYPE_OFF)
#define GPIO0_INTPOL        REG32(GPIO0_BASE + GPIO_INTPOL_OFF)
#define GPIO0_INTSTAT       REG32(GPIO0_BASE + GPIO_INTSTAT_OFF)
#define GPIO0_EOI           REG32(GPIO0_BASE + GPIO_EOI_OFF)

/* Convenience macros for GPIO1 */
#define GPIO1_DR            REG32(GPIO1_BASE + GPIO_DR_OFF)
#define GPIO1_DDR           REG32(GPIO1_BASE + GPIO_DDR_OFF)
#define GPIO1_EXT           REG32(GPIO1_BASE + GPIO_EXT_OFF)
#define GPIO1_INTEN         REG32(GPIO1_BASE + GPIO_INTEN_OFF)
#define GPIO1_INTMASK       REG32(GPIO1_BASE + GPIO_INTMASK_OFF)
#define GPIO1_INTTYPE       REG32(GPIO1_BASE + GPIO_INTTYPE_OFF)
#define GPIO1_INTPOL        REG32(GPIO1_BASE + GPIO_INTPOL_OFF)
#define GPIO1_INTSTAT       REG32(GPIO1_BASE + GPIO_INTSTAT_OFF)
#define GPIO1_EOI           REG32(GPIO1_BASE + GPIO_EOI_OFF)

/* GPIO1 bit positions for specific signals */
#define GPIO1_BIT_GPIO16    0   /* WIFI_EN  - reserved                        */
#define GPIO1_BIT_GPIO17    1   /* WIFI_RST - reserved                        */
#define GPIO1_BIT_GPIO18    2
#define GPIO1_BIT_GPIO19    3
#define GPIO1_BIT_GPIO20    4
#define GPIO1_BIT_GPIO21    5
#define GPIO1_BIT_GPIO22    6   /* USER_LED */
#define GPIO1_BIT_GPIO23    7
#define GPIO1_BIT_GPIO24    8
/* GPIO1[9:15] = GPIO25..GPIO31  -- used by baud-rate divider, DO NOT touch */

/* -----------------------------------------------------------------------
 * UART  (DesignWare 16550-compatible APB UART)
 * DLAB=0: RBR(R)/THR(W) at 0x00,  IER at 0x04
 * DLAB=1: DLL at 0x00, DLH at 0x04
 * ----------------------------------------------------------------------- */
#define UART_RBR_OFF        0x00   /* Receive Buffer Register   (read,  DLAB=0) */
#define UART_THR_OFF        0x00   /* Transmit Holding Register (write, DLAB=0) */
#define UART_DLL_OFF        0x00   /* Divisor Latch Low         (DLAB=1)        */
#define UART_DLH_OFF        0x04   /* Divisor Latch High        (DLAB=1)        */
#define UART_IER_OFF        0x04   /* Interrupt Enable Register (DLAB=0)        */
#define UART_IIR_OFF        0x08   /* Interrupt Identification  (read)          */
#define UART_FCR_OFF        0x08   /* FIFO Control Register     (write)         */
#define UART_LCR_OFF        0x0C   /* Line Control Register                     */
#define UART_MCR_OFF        0x10   /* Modem Control Register                    */
#define UART_LSR_OFF        0x14   /* Line Status Register                      */
#define UART_MSR_OFF        0x18   /* Modem Status Register                     */
#define UART_SCR_OFF        0x1C   /* Scratch Register                          */

/* LCR bits */
#define UART_LCR_WLS_8      0x03   /* 8-bit word length                         */
#define UART_LCR_STB_1      0x00   /* 1 stop bit                                */
#define UART_LCR_PEN        0x08   /* Parity Enable                             */
#define UART_LCR_EPS        0x10   /* Even Parity Select                        */
#define UART_LCR_DLAB       0x80   /* Divisor Latch Access Bit                  */

/* FCR bits */
#define UART_FCR_FIFOEN     0x01   /* FIFO Enable                               */
#define UART_FCR_RXCLR      0x02   /* Clear RX FIFO                             */
#define UART_FCR_TXCLR      0x04   /* Clear TX FIFO                             */
#define UART_FCR_RXTRIG_1   0x00   /* RX trigger: 1 byte                        */
#define UART_FCR_RXTRIG_4   0x40
#define UART_FCR_RXTRIG_8   0x80
#define UART_FCR_RXTRIG_14  0xC0

/* IER bits */
#define UART_IER_RDI        0x01   /* Enable RX Data Available Interrupt        */
#define UART_IER_THRI       0x02   /* Enable TX Holding Reg Empty Interrupt     */
#define UART_IER_RLSI       0x04   /* Enable RX Line Status Interrupt           */
#define UART_IER_MSI        0x08   /* Enable Modem Status Interrupt             */

/* LSR bits */
#define UART_LSR_DR         0x01   /* Data Ready (RX has data)                  */
#define UART_LSR_OE         0x02   /* Overrun Error                             */
#define UART_LSR_PE         0x04   /* Parity Error                              */
#define UART_LSR_FE         0x08   /* Framing Error                             */
#define UART_LSR_BI         0x10   /* Break Interrupt                           */
#define UART_LSR_THRE       0x20   /* TX Holding Register Empty                 */
#define UART_LSR_TEMT       0x40   /* Transmitter Empty                         */

/* Baud rate divisor: divisor = F_CPU / (16 * baud)                         */
#define UART_BAUD_DIV(baud) ((uint32_t)(F_CPU / (16UL * (baud))))

/* Per-UART register access */
#define UART_REG(base, off) REG32((base) + (off))

/* -----------------------------------------------------------------------
 * SPI  (DesignWare DW_apb_ssi)
 * ----------------------------------------------------------------------- */
#define SPI_CTRLR0_OFF      0x00
#define SPI_CTRLR1_OFF      0x04
#define SPI_SSIENR_OFF      0x08
#define SPI_MWCR_OFF        0x0C
#define SPI_SER_OFF         0x10   /* Slave Enable Register                     */
#define SPI_BAUDR_OFF       0x14   /* Baud Rate Select  (sclk = f_cpu/BAUDR)    */
#define SPI_TXFTLR_OFF      0x18   /* TX FIFO Threshold Level                   */
#define SPI_RXFTLR_OFF      0x1C   /* RX FIFO Threshold Level                   */
#define SPI_TXFLR_OFF       0x20   /* TX FIFO Level (current entries)           */
#define SPI_RXFLR_OFF       0x24   /* RX FIFO Level                             */
#define SPI_SR_OFF          0x28   /* Status Register                           */
#define SPI_IMR_OFF         0x2C   /* Interrupt Mask                            */
#define SPI_ISR_OFF         0x30   /* Interrupt Status                          */
#define SPI_RISR_OFF        0x34   /* Raw Interrupt Status                      */
#define SPI_TXOICR_OFF      0x38
#define SPI_RXOICR_OFF      0x3C
#define SPI_RXUICR_OFF      0x40
#define SPI_MSTICR_OFF      0x44
#define SPI_ICR_OFF         0x48
#define SPI_DR_OFF          0x60   /* Data Register (FIFO, 16 deep)             */

/* CTRLR0 fields */
#define SPI_CTRLR0_DFS(n)   ((n) & 0xF)          /* Data Frame Size: 7=8bit   */
#define SPI_CTRLR0_FRF_MOTO (0x0 << 4)            /* Motorola SPI frame format  */
#define SPI_CTRLR0_SCPH     (1 << 6)              /* Clock Phase                */
#define SPI_CTRLR0_SCPOL    (1 << 7)              /* Clock Polarity             */
#define SPI_CTRLR0_TMOD_TX  (0x1 << 8)            /* TX only                    */
#define SPI_CTRLR0_TMOD_RX  (0x2 << 8)            /* RX only                    */
#define SPI_CTRLR0_TMOD_TR  (0x0 << 8)            /* TX + RX                    */

/* SR (Status Register) bits */
#define SPI_SR_BUSY         0x01
#define SPI_SR_TFNF         0x02   /* TX FIFO Not Full                          */
#define SPI_SR_TFE          0x04   /* TX FIFO Empty                             */
#define SPI_SR_RFNE         0x08   /* RX FIFO Not Empty                         */
#define SPI_SR_RFF          0x10   /* RX FIFO Full                              */

/* SPI clock: sclk_out = f_cpu / BAUDR  (must be even, min 2)                */
#define SPI_BAUDR_VAL(hz)   ((uint32_t)(F_CPU / (hz)))

#define SPI_REG(base, off)  REG32((base) + (off))

/* -----------------------------------------------------------------------
 * I2C  (DesignWare DW_apb_i2c)
 * ----------------------------------------------------------------------- */
#define I2C_CON_OFF         0x00
#define I2C_TAR_OFF         0x04   /* Target Address                            */
#define I2C_SAR_OFF         0x08   /* Slave Address                             */
#define I2C_DATA_CMD_OFF    0x10   /* Data + Command                            */
#define I2C_SS_SCL_HCNT_OFF 0x14   /* Std Speed SCL High Count                  */
#define I2C_SS_SCL_LCNT_OFF 0x18   /* Std Speed SCL Low Count                   */
#define I2C_FS_SCL_HCNT_OFF 0x1C   /* Fast Speed SCL High Count                 */
#define I2C_FS_SCL_LCNT_OFF 0x20   /* Fast Speed SCL Low Count                  */
#define I2C_INTR_STAT_OFF   0x2C
#define I2C_INTR_MASK_OFF   0x30
#define I2C_RAW_INTR_OFF    0x34
#define I2C_RX_TL_OFF       0x38
#define I2C_TX_TL_OFF       0x3C
#define I2C_CLR_INTR_OFF    0x40
#define I2C_CLR_TX_ABRT_OFF 0x54
#define I2C_CLR_STOP_DET    0x60
#define I2C_ENABLE_OFF      0x6C
#define I2C_STATUS_OFF      0x70
#define I2C_TXFLR_OFF       0x74
#define I2C_RXFLR_OFF       0x78
#define I2C_TX_ABRT_SRC_OFF 0x80
#define I2C_ENABLE_STAT_OFF 0x9C

/* CON register bits */
#define I2C_CON_MASTER          (1 << 0)
#define I2C_CON_SPEED_STD       (1 << 1)   /* Standard speed 100kHz             */
#define I2C_CON_SPEED_FAST      (2 << 1)   /* Fast speed 400kHz                 */
#define I2C_CON_10BIT_SLAVE     (1 << 3)
#define I2C_CON_10BIT_MASTER    (1 << 4)
#define I2C_CON_RESTART_EN      (1 << 5)
#define I2C_CON_SLAVE_DISABLE   (1 << 6)

/* DATA_CMD register bits */
#define I2C_DATA_CMD_READ       (1 << 8)   /* 1=read, 0=write                   */
#define I2C_DATA_CMD_STOP       (1 << 9)   /* Issue STOP after this byte        */
#define I2C_DATA_CMD_RESTART    (1 << 10)  /* Issue RESTART before this byte    */

/* STATUS register bits */
#define I2C_STATUS_ACTIVITY     (1 << 0)
#define I2C_STATUS_TFNF         (1 << 1)   /* TX FIFO Not Full                  */
#define I2C_STATUS_TFE          (1 << 2)   /* TX FIFO Empty                     */
#define I2C_STATUS_RFNE         (1 << 3)   /* RX FIFO Not Empty                 */
#define I2C_STATUS_MST_ACTIVITY (1 << 5)   /* Master FSM not idle               */

/* RAW_INTR_STAT bits */
#define I2C_RAW_TX_ABRT         (1 << 6)
#define I2C_RAW_STOP_DET        (1 << 9)
#define I2C_RAW_RX_FULL         (1 << 2)
#define I2C_RAW_RX_OVER         (1 << 1)

/*
 * SCL clock counts for 100 MHz system clock:
 * Standard (100kHz): HCNT = f_sys/(2*f_scl) - 7  = 493
 *                    LCNT = f_sys/(2*f_scl) - 1  = 499
 * Fast     (400kHz): HCNT = f_sys/(2*f_scl) - 7  = 118
 *                    LCNT = f_sys/(2*f_scl) - 1  = 124
 */
#define I2C_SS_HCNT         493
#define I2C_SS_LCNT         499
#define I2C_FS_HCNT         118
#define I2C_FS_LCNT         124

#define I2C_REG(base, off)  REG32((base) + (off))

/* -----------------------------------------------------------------------
 * TIMER  (DesignWare APB Timer)
 * Three 32-bit timers. Each occupies 0x14 bytes.
 * ----------------------------------------------------------------------- */
#define TIMER_LOADCOUNT_OFF     0x00   /* Load count value                      */
#define TIMER_CURRENTVAL_OFF    0x04   /* Current counter value (read-only)     */
#define TIMER_CONTROLREG_OFF    0x08
#define TIMER_EOI_OFF           0x0C   /* Read to clear interrupt               */
#define TIMER_INTSTATUS_OFF     0x10

/* CONTROLREG bits */
#define TIMER_CTRL_ENABLE       (1 << 0)
#define TIMER_CTRL_MODE_USER    (1 << 1)   /* 1=user-defined count (periodic)   */
#define TIMER_CTRL_INTMASK      (1 << 2)   /* 1=interrupt masked (disabled)     */

/* Global timer registers */
#define TIMERS_INTSTATUS_REG    REG32(TIMERS_INTSTATUS)
#define TIMERS_EOI_REG          REG32(TIMERS_EOI)
#define TIMERS_RAWSTATUS_REG    REG32(TIMERS_RAWSTATUS)

/* Per-timer register access */
#define TIMER_REG(base, off)    REG32((base) + (off))

/*
 * Timer reload value for 1 ms tick at 100 MHz:
 * LOAD = F_CPU / 1000 - 1 = 99999
 */
#define TIMER_1MS_LOAD          (F_CPU / 1000UL - 1UL)
#define TIMER_1US_LOAD          (F_CPU / 1000000UL - 1UL)

/* -----------------------------------------------------------------------
 * PWM  (Custom THEJAS32 PWM Controller)
 * 8 channels, each channel occupies 0x10 bytes from PWM_BASE
 * ----------------------------------------------------------------------- */
#define PWM_CH_OFF(n)           ((n) * 0x10)
#define PWM_CTRL_OFF            0x00
#define PWM_PERIOD_OFF          0x04   /* Total period in clock cycles           */
#define PWM_DUTY_OFF            0x08   /* High-time in clock cycles              */
#define PWM_STATUS_OFF          0x0C

/* CTRL register bits */
#define PWM_CTRL_ENABLE         (1 << 0)
#define PWM_CTRL_ONESHOT        (1 << 1)   /* 0=continuous (default)            */
#define PWM_CTRL_INTEN          (1 << 2)   /* Interrupt enable                  */
#define PWM_CTRL_RIGHT_ALIGN    (1 << 3)   /* 0=left-aligned (default)          */

/* Per-channel register access */
#define PWM_CH_REG(n, off)      REG32(PWM_BASE + PWM_CH_OFF(n) + (off))
#define PWM_NUM_CHANNELS        8

/*
 * Default PWM frequency: 1 kHz
 * PERIOD = F_CPU / freq = 100 000 000 / 1000 = 100 000 cycles
 * DUTY for analogWrite(pin, val): PERIOD * val / 255
 */
#define PWM_DEFAULT_FREQ_HZ     1000UL
#define PWM_PERIOD_CYCLES       (F_CPU / PWM_DEFAULT_FREQ_HZ)

/* -----------------------------------------------------------------------
 * PLIC  (Platform Level Interrupt Controller)
 * ----------------------------------------------------------------------- */
#define PLIC_PRIORITY_BASE      (PLIC_BASE + 0x0004UL)   /* priority[1..31]    */
#define PLIC_PENDING_BASE       (PLIC_BASE + 0x1000UL)   /* pending bits        */
#define PLIC_ENABLE_BASE        (PLIC_BASE + 0x2000UL)   /* enable bits hart 0  */
#define PLIC_THRESHOLD          (PLIC_BASE + 0x200000UL) /* priority threshold  */
#define PLIC_CLAIM              (PLIC_BASE + 0x200004UL) /* claim / complete    */

/* Interrupt source numbers (from memory map) */
#define IRQ_UART0               0
#define IRQ_UART1               1
#define IRQ_UART2               2
#define IRQ_SPI0                3
#define IRQ_SPI1                4
#define IRQ_I2C0                5
#define IRQ_I2C1                6
#define IRQ_TIMER0              7
#define IRQ_TIMER1              8
#define IRQ_TIMER2              9
/* GPIO0[0:11] → IRQ 10..21, GPIO1 misc → IRQ 22 */
#define IRQ_GPIO0_BASE          10
#define IRQ_SPI2                23
#define IRQ_PWM0                24   /* PWM[0:7] → IRQ 24..31 */
#define IRQ_PWM_BASE            24

/* PLIC helper macros */
#define PLIC_SET_PRIORITY(irq, pri)  \
    REG32(PLIC_PRIORITY_BASE + ((irq) - 1) * 4) = (pri)
#define PLIC_ENABLE_IRQ(irq)         \
    REG32(PLIC_ENABLE_BASE + (((irq) / 32) * 4)) |= (1U << ((irq) % 32))
#define PLIC_DISABLE_IRQ(irq)        \
    REG32(PLIC_ENABLE_BASE + (((irq) / 32) * 4)) &= ~(1U << ((irq) % 32))
#define PLIC_SET_THRESHOLD(t)        \
    REG32(PLIC_THRESHOLD) = (t)
#define PLIC_CLAIM_IRQ()             \
    REG32(PLIC_CLAIM)
#define PLIC_COMPLETE_IRQ(irq)       \
    REG32(PLIC_CLAIM) = (irq)

/* -----------------------------------------------------------------------
 * RISC-V CSR helpers (RV32IM)
 * ----------------------------------------------------------------------- */
#define csr_read(reg)           ({ uint32_t _v; \
    __asm__ volatile("csrr %0, " #reg : "=r"(_v)); _v; })

#define csr_write(reg, val)     \
    __asm__ volatile("csrw " #reg ", %0" :: "r"((uint32_t)(val)))

#define csr_set(reg, bits)      \
    __asm__ volatile("csrs " #reg ", %0" :: "r"((uint32_t)(bits)))

#define csr_clear(reg, bits)    \
    __asm__ volatile("csrc " #reg ", %0" :: "r"((uint32_t)(bits)))

/* Machine-mode CSR names */
#define MSTATUS_MIE     (1U << 3)   /* global machine interrupt enable          */
#define MSTATUS_MPIE    (1U << 7)
#define MIE_MEIE        (1U << 11)  /* external interrupt enable                */
#define MIE_MTIE        (1U << 7)   /* machine timer interrupt enable           */
#define MIE_MSIE        (1U << 3)   /* machine software interrupt enable        */
#define MCAUSE_IRQ      (1U << 31)  /* set when mcause is an interrupt          */
#define MCAUSE_M_EXT    11          /* machine external interrupt cause         */
#define MCAUSE_M_TIMER  7           /* machine timer interrupt cause            */

/* Enable / disable all machine interrupts */
#define enable_irq()    csr_set(mstatus, MSTATUS_MIE)
#define disable_irq()   csr_clear(mstatus, MSTATUS_MIE)

/* Read 64-bit cycle counter (rdcycle + rdcycleh) */
static inline uint64_t read_cycle64(void) {
    uint32_t lo, hi, hi2;
    do {
        __asm__ volatile("rdcycleh %0" : "=r"(hi));
        __asm__ volatile("rdcycle  %0" : "=r"(lo));
        __asm__ volatile("rdcycleh %0" : "=r"(hi2));
    } while (hi != hi2);
    return ((uint64_t)hi << 32) | lo;
}

/* Read 32-bit cycle counter (fast, wraps at ~42 seconds @ 100MHz) */
static inline uint32_t read_cycle(void) {
    uint32_t v;
    __asm__ volatile("rdcycle %0" : "=r"(v));
    return v;
}

/* Busy-wait using cycle counter */
static inline void busy_wait_cycles(uint32_t cycles) {
    uint32_t start = read_cycle();
    while ((read_cycle() - start) < cycles);
}

/* -----------------------------------------------------------------------
 * Utility
 * ----------------------------------------------------------------------- */
#define BIT(n)          (1U << (n))
#define SETBIT(r, n)    ((r) |= BIT(n))
#define CLRBIT(r, n)    ((r) &= ~BIT(n))
#define TESTBIT(r, n)   (((r) >> (n)) & 1U)

#endif /* THEJAS32_H */
