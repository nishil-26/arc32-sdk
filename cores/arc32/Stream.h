/*
 * Stream.h  —  Arc32 Stream base class
 * Adds read-side helpers (available/read/peek + simple parsing) on top of Print.
 */
#ifndef STREAM_H
#define STREAM_H

#include "Print.h"

class Stream : public Print {
public:
    Stream() : _timeout(1000) {}
    virtual ~Stream() {}

    virtual int  available(void) = 0;
    virtual int  read(void)      = 0;
    virtual int  peek(void)      = 0;
    virtual void flush(void)     = 0;

    void setTimeout(unsigned long ms) { _timeout = ms; }

    /* Blocking read of exactly `len` bytes (or until timeout) */
    size_t readBytes(char *buf, size_t len);
    size_t readBytes(uint8_t *buf, size_t len) {
        return readBytes((char *)buf, len);
    }

    /* Read until terminator or timeout, return bytes read */
    size_t readBytesUntil(char terminator, char *buf, size_t len);

    /* Parse next integer from stream (skips leading non-digits) */
    long parseInt(void);
    float parseFloat(void);

    /* Wait for and discard a specific byte; returns true if found */
    bool find(const char *target);

protected:
    unsigned long _timeout;
    int timedRead(void);   /* returns -1 on timeout */
};

#endif /* STREAM_H */
