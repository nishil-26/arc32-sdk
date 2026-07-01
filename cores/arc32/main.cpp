/*
 * main.cpp  —  Arc32 Arduino Core Entry Point
 *
 * Called from _start (startup.S) after .bss/.data are set up.
 * Runs C++ static constructors, calls init() for hardware bring-up,
 * then calls the sketch's setup() once and loop() forever.
 */

#include "Arduino.h"

extern "C" {
    /* Provided by linker script */
    extern void (*__init_array_start[])(void);
    extern void (*__init_array_end[])(void);

    int main(void)
    {
        /* ---- Run C++ global object constructors ---- */
        for (void (**ctor)(void) = __init_array_start;
             ctor < __init_array_end; ctor++)
        {
            (*ctor)();
        }

        /* ---- Hardware bring-up: TIMER0 for millis(), PLIC, IRQ enable ---- */
        init();

        /* ---- User sketch ---- */
        setup();

        for (;;) {
            loop();
        }

        return 0;   /* never reached */
    }
}
