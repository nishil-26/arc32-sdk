/*
 * WString.h  —  Arc32 String class
 * A minimal but functional Arduino-compatible String, heap-backed.
 */
#ifndef WSTRING_H
#define WSTRING_H

#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>

class String {
public:
    String();
    String(const char *cstr);
    String(const String &s);
    String(char c);
    String(int n, int base = 10);
    String(unsigned int n, int base = 10);
    String(long n, int base = 10);
    String(unsigned long n, int base = 10);
    String(float n, int digits = 2);
    String(double n, int digits = 2);
    ~String();

    /* Assignment */
    String &operator=(const String &rhs);
    String &operator=(const char *cstr);

    /* Concatenation */
    String &operator+=(const String &rhs);
    String &operator+=(const char *cstr);
    String &operator+=(char c);
    String &operator+=(int n);
    String &operator+=(unsigned int n);
    String &operator+=(long n);
    String &operator+=(unsigned long n);
    String &operator+=(float n);
    String &operator+=(double n);

    friend String operator+(String lhs, const String &rhs) { lhs += rhs; return lhs; }
    friend String operator+(String lhs, const char *rhs)   { lhs += rhs; return lhs; }
    friend String operator+(String lhs, char rhs)          { lhs += rhs; return lhs; }

    /* Comparison */
    bool operator==(const String &rhs) const;
    bool operator==(const char *cstr) const;
    bool operator!=(const String &rhs) const { return !(*this == rhs); }
    bool operator<(const String &rhs) const;

    /* Indexing */
    char  operator[](unsigned int i) const;
    char &operator[](unsigned int i);

    /* Access */
    const char *c_str(void) const { return _buf ? _buf : ""; }
    unsigned int length(void) const { return _len; }
    bool isEmpty(void) const { return _len == 0; }

    /* Manipulation */
    void  toCharArray(char *buf, unsigned int bufsize) const;
    long  toInt(void) const;
    float toFloat(void) const;
    double toDouble(void) const;
    String substring(unsigned int from) const;
    String substring(unsigned int from, unsigned int to) const;
    int   indexOf(char c) const;
    int   indexOf(const char *str) const;
    int   indexOf(char c, unsigned int fromIndex) const;
    void  replace(const String &find, const String &repl);
    void  trim(void);
    void  toUpperCase(void);
    void  toLowerCase(void);
    bool  startsWith(const String &prefix) const;
    bool  endsWith(const String &suffix) const;
    bool  equalsIgnoreCase(const String &s) const;
    bool  concat(const String &s);
    bool  concat(const char *cstr);
    bool  reserve(unsigned int size);

private:
    char        *_buf;
    unsigned int _len;
    unsigned int _cap;

    void init(void);
    bool copy(const char *cstr, unsigned int length);
    void invalidate(void);
};

#endif /* WSTRING_H */
