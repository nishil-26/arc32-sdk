/*
 * Print.cpp  —  Arc32 Print base class implementation
 */
#include "Print.h"
#include "WString.h"
#include <stdlib.h>
#include <string.h>

size_t Print::write(const uint8_t *buf, size_t len)
{
    size_t n = 0;
    for (size_t i = 0; i < len; i++) n += write(buf[i]);
    return n;
}

size_t Print::write(const char *str)
{
    if (!str) return 0;
    return write((const uint8_t *)str, strlen(str));
}

size_t Print::write(const char *buf, size_t len)
{
    return write((const uint8_t *)buf, len);
}

size_t Print::print(const char *str) { return write(str); }

size_t Print::print(char c) { return write((uint8_t)c); }

size_t Print::print(const String &s) { return write(s.c_str(), s.length()); }

size_t Print::printNumber(unsigned long n, uint8_t base)
{
    char buf[34];   /* enough for 32-bit binary + sign */
    char *p = buf + sizeof(buf) - 1;
    *p = '\0';

    if (n == 0) {
        *--p = '0';
    } else {
        while (n > 0) {
            uint8_t digit = n % base;
            *--p = (digit < 10) ? ('0' + digit) : ('A' + digit - 10);
            n /= base;
        }
    }
    return write(p);
}

size_t Print::print(unsigned char b, int base)  { return printNumber(b, base); }
size_t Print::print(unsigned int n, int base)   { return printNumber(n, base); }
size_t Print::print(unsigned long n, int base)  { return printNumber(n, base); }

size_t Print::print(int n, int base)
{
    if (base == 10 && n < 0) {
        size_t t = write('-');
        return t + printNumber((unsigned long)(-n), 10);
    }
    return printNumber((unsigned long)n, base);
}

size_t Print::print(long n, int base)
{
    if (base == 10 && n < 0) {
        size_t t = write('-');
        return t + printNumber((unsigned long)(-n), 10);
    }
    return printNumber((unsigned long)n, base);
}

size_t Print::printFloat(double n, int digits)
{
    size_t total = 0;
    if (n < 0) { total += write('-'); n = -n; }

    /* Round to requested digits */
    double rounding = 0.5;
    for (int i = 0; i < digits; i++) rounding /= 10.0;
    n += rounding;

    unsigned long int_part = (unsigned long)n;
    double frac = n - (double)int_part;

    total += printNumber(int_part, 10);

    if (digits > 0) {
        total += write('.');
        while (digits-- > 0) {
            frac *= 10.0;
            unsigned int d = (unsigned int)frac;
            total += write((char)('0' + d));
            frac -= d;
        }
    }
    return total;
}

size_t Print::print(double n, int digits) { return printFloat(n, digits); }

/* ----- println variants ----- */
size_t Print::println(void) { return write("\r\n"); }

size_t Print::println(const char *str) {
    size_t n = print(str); return n + println();
}
size_t Print::println(char c) {
    size_t n = print(c); return n + println();
}
size_t Print::println(unsigned char b, int base) {
    size_t n = print(b, base); return n + println();
}
size_t Print::println(int v, int base) {
    size_t n = print(v, base); return n + println();
}
size_t Print::println(unsigned int v, int base) {
    size_t n = print(v, base); return n + println();
}
size_t Print::println(long v, int base) {
    size_t n = print(v, base); return n + println();
}
size_t Print::println(unsigned long v, int base) {
    size_t n = print(v, base); return n + println();
}
size_t Print::println(double v, int digits) {
    size_t n = print(v, digits); return n + println();
}
size_t Print::println(const String &s) {
    size_t n = print(s); return n + println();
}
