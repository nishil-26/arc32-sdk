/*
 * cxx_support.cpp  —  Arc32 Minimal C++ Runtime Support
 *
 * Because Arc32 sketches are compiled with -fno-exceptions -fno-rtti and
 * NOT linked against libstdc++ (no port exists for bare-metal RISC-V
 * rv32im in this toolchain), the handful of low-level C++ runtime hooks
 * that the compiler still emits references to (operator new/delete,
 * pure virtual call handler) must be supplied manually here.
 *
 * Memory backing: picolibc's malloc/free (same heap used by String).
 */

#include <stdlib.h>
#include <stddef.h>

void *operator new(size_t size)
{
    return malloc(size);
}

void *operator new[](size_t size)
{
    return malloc(size);
}

void operator delete(void *ptr) noexcept
{
    free(ptr);
}

void operator delete[](void *ptr) noexcept
{
    free(ptr);
}

/* Sized-delete overloads (C++14) — some GCC versions emit calls to these
 * instead of the unsized form depending on optimization level. */
void operator delete(void *ptr, size_t) noexcept
{
    free(ptr);
}

void operator delete[](void *ptr, size_t) noexcept
{
    free(ptr);
}

/*
 * Called if a pure virtual function is ever invoked (should never happen
 * in correct code, but the symbol must exist or linking fails whenever
 * any class has pure virtual methods — e.g. Stream's abstract interface).
 */
extern "C" void __cxa_pure_virtual(void)
{
    /* Nothing sensible to do on bare metal — halt. */
    for (;;) { /* trap */ }
}

/*
 * Stub for static local variable guards (thread-safe initialization).
 * Arc32 is single-threaded (no RTOS in v1.0), so these are no-ops.
 */
extern "C" int __cxa_guard_acquire(long *g)
{
    return !(*g);
}
extern "C" void __cxa_guard_release(long *g)
{
    *g = 1;
}
extern "C" void __cxa_guard_abort(long *) { /* nothing */ }
