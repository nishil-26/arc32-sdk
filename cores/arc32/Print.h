/*
 * Print.h  —  Arc32 Print base class
 * Provides print()/println() for all types, built on virtual write().
 */
#ifndef PRINT_H
#define PRINT_H

#include <stdint.h>
#include <stddef.h>

class String;   /* forward declaration (WString.h) */

class Print {
public:
    Print() : _decimal_places(2) {}
    virtual ~Print() {}

    /* Must be implemented by derived class */
    virtual size_t write(uint8_t b) = 0;
    virtual size_t write(const uint8_t *buf, size_t len);
    size_t write(const char *str);
    size_t write(const char *buf, size_t len);

    size_t print(const char *str);
    size_t print(char c);
    size_t print(unsigned char b, int base = 10);
    size_t print(int n, int base = 10);
    size_t print(unsigned int n, int base = 10);
    size_t print(long n, int base = 10);
    size_t print(unsigned long n, int base = 10);
    size_t print(double n, int digits = 2);
    size_t print(const String &s);

    size_t println(const char *str);
    size_t println(char c);
    size_t println(unsigned char b, int base = 10);
    size_t println(int n, int base = 10);
    size_t println(unsigned int n, int base = 10);
    size_t println(long n, int base = 10);
    size_t println(unsigned long n, int base = 10);
    size_t println(double n, int digits = 2);
    size_t println(const String &s);
    size_t println(void);

private:
    int _decimal_places;
    size_t printNumber(unsigned long n, uint8_t base);
    size_t printFloat(double n, int digits);
};

#endif /* PRINT_H */
