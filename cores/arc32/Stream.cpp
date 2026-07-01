/*
 * Stream.cpp  —  Arc32 Stream base class implementation
 */
#include "Stream.h"
#include "Arduino.h"

int Stream::timedRead(void)
{
    unsigned long start = millis();
    int c;
    do {
        c = read();
        if (c >= 0) return c;
    } while ((millis() - start) < _timeout);
    return -1;   /* timeout */
}

size_t Stream::readBytes(char *buf, size_t len)
{
    size_t count = 0;
    while (count < len) {
        int c = timedRead();
        if (c < 0) break;
        buf[count++] = (char)c;
    }
    return count;
}

size_t Stream::readBytesUntil(char terminator, char *buf, size_t len)
{
    size_t count = 0;
    while (count < len) {
        int c = timedRead();
        if (c < 0 || c == terminator) break;
        buf[count++] = (char)c;
    }
    return count;
}

long Stream::parseInt(void)
{
    bool negative = false;
    long value = 0;
    int c;

    /* Skip non-numeric characters (but watch for '-') */
    do {
        c = timedRead();
        if (c == '-') negative = true;
    } while (c != -1 && (c < '0' || c > '9') && c != '-');

    while (c >= '0' && c <= '9') {
        value = value * 10 + (c - '0');
        c = timedRead();
    }

    return negative ? -value : value;
}

float Stream::parseFloat(void)
{
    bool negative = false;
    double value = 0;
    int c;

    do {
        c = timedRead();
        if (c == '-') negative = true;
    } while (c != -1 && (c < '0' || c > '9') && c != '-' && c != '.');

    while (c >= '0' && c <= '9') {
        value = value * 10 + (c - '0');
        c = timedRead();
    }

    if (c == '.') {
        double frac = 0.1;
        c = timedRead();
        while (c >= '0' && c <= '9') {
            value += (c - '0') * frac;
            frac *= 0.1;
            c = timedRead();
        }
    }

    return negative ? -(float)value : (float)value;
}

bool Stream::find(const char *target)
{
    size_t tlen = 0;
    while (target[tlen]) tlen++;
    if (tlen == 0) return true;

    size_t matched = 0;
    while (true) {
        int c = timedRead();
        if (c < 0) return false;
        if ((char)c == target[matched]) {
            matched++;
            if (matched == tlen) return true;
        } else {
            matched = ((char)c == target[0]) ? 1 : 0;
        }
    }
}
