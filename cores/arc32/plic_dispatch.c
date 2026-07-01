/*
 * plic_dispatch.c  —  Arc32 PLIC Interrupt Dispatcher
 *
 * Called from trap_handler (startup.S) on every Machine External Interrupt.
 * Claims the highest-priority pending IRQ from the PLIC, dispatches it
 * to the correct peripheral ISR, then signals completion.
 */

#include "thejas32.h"

/* Forward declarations of peripheral ISRs (defined in their own .c files) */
extern void _timer0_isr(void);
extern void _uart0_isr(void);
extern void _uart1_isr(void);
extern void _uart2_isr(void);
extern void _gpio0_irq_dispatch(void);

/* Weak default ISR for unhandled/unused interrupt sources */
void _default_isr(void) { /* nothing */ }

void _plic_dispatch(void)
{
    /* Claim the pending interrupt with highest priority */
    uint32_t irq = PLIC_CLAIM_IRQ();

    if (irq == 0) return;   /* spurious - nothing pending */

    switch (irq) {
        case IRQ_UART0:  _uart0_isr();  break;
        case IRQ_UART1:  _uart1_isr();  break;
        case IRQ_UART2:  _uart2_isr();  break;
        case IRQ_TIMER0: _timer0_isr(); break;

        case IRQ_GPIO0_BASE + 0:  case IRQ_GPIO0_BASE + 1:
        case IRQ_GPIO0_BASE + 2:  case IRQ_GPIO0_BASE + 3:
        case IRQ_GPIO0_BASE + 4:  case IRQ_GPIO0_BASE + 5:
        case IRQ_GPIO0_BASE + 6:  case IRQ_GPIO0_BASE + 7:
        case IRQ_GPIO0_BASE + 8:  case IRQ_GPIO0_BASE + 9:
        case IRQ_GPIO0_BASE + 10: case IRQ_GPIO0_BASE + 11:
            _gpio0_irq_dispatch();
            break;

        default:
            _default_isr();
            break;
    }

    /* Tell the PLIC we're done with this IRQ */
    PLIC_COMPLETE_IRQ(irq);
}
