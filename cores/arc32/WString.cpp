/*
 * WString.cpp  —  Arc32 String class implementation
 */
#include "WString.h"
#include <stdio.h>
#include <ctype.h>

void String::init(void) { _buf = NULL; _len = 0; _cap = 0; }

void String::invalidate(void) {
    if (_buf) free(_buf);
    init();
}

bool String::reserve(unsigned int size)
{
    if (_cap >= size) return true;
    char *newbuf = (char *)realloc(_buf, size + 1);
    if (!newbuf) return false;
    _buf = newbuf;
    _cap = size;
    return true;
}

bool String::copy(const char *cstr, unsigned int length)
{
    if (!reserve(length)) return false;
    memcpy(_buf, cstr, length);
    _buf[length] = '\0';
    _len = length;
    return true;
}

/* ----- Constructors ----- */
String::String() { init(); }

String::String(const char *cstr) {
    init();
    if (cstr) copy(cstr, strlen(cstr));
    else copy("", 0);
}

String::String(const String &s) {
    init();
    copy(s._buf ? s._buf : "", s._len);
}

String::String(char c) {
    init();
    char buf[2] = { c, '\0' };
    copy(buf, 1);
}

String::String(int n, int base) {
    init();
    char buf[34];
    if (base == 10) snprintf(buf, sizeof(buf), "%d", n);
    else { /* manual base conversion */
        bool neg = n < 0;
        unsigned int un = neg ? (unsigned int)(-n) : (unsigned int)n;
        char tmp[34]; int i = 0;
        if (un == 0) tmp[i++] = '0';
        while (un > 0) { int d = un % base; tmp[i++] = d<10?('0'+d):('A'+d-10); un/=base; }
        int j = 0;
        if (neg) buf[j++] = '-';
        while (i > 0) buf[j++] = tmp[--i];
        buf[j] = '\0';
    }
    copy(buf, strlen(buf));
}

String::String(unsigned int n, int base) {
    init();
    char buf[34]; int i = 0;
    if (n == 0) buf[i++] = '0';
    while (n > 0) { int d = n % base; buf[i++] = d<10?('0'+d):('A'+d-10); n/=base; }
    char rev[34]; int j = 0;
    while (i > 0) rev[j++] = buf[--i];
    rev[j] = '\0';
    copy(rev, j);
}

String::String(long n, int base) {
    init();
    char buf[34];
    if (base == 10) snprintf(buf, sizeof(buf), "%ld", n);
    else {
        bool neg = n < 0;
        unsigned long un = neg ? (unsigned long)(-n) : (unsigned long)n;
        char tmp[34]; int i = 0;
        if (un == 0) tmp[i++] = '0';
        while (un > 0) { int d = un % base; tmp[i++] = d<10?('0'+d):('A'+d-10); un/=base; }
        int j = 0;
        if (neg) buf[j++] = '-';
        while (i > 0) buf[j++] = tmp[--i];
        buf[j] = '\0';
    }
    copy(buf, strlen(buf));
}

String::String(unsigned long n, int base) {
    init();
    char buf[34]; int i = 0;
    if (n == 0) buf[i++] = '0';
    while (n > 0) { int d = n % base; buf[i++] = d<10?('0'+d):('A'+d-10); n/=base; }
    char rev[34]; int j = 0;
    while (i > 0) rev[j++] = buf[--i];
    rev[j] = '\0';
    copy(rev, j);
}

String::String(float n, int digits) {
    init();
    char buf[34];
    snprintf(buf, sizeof(buf), "%.*f", digits, (double)n);
    copy(buf, strlen(buf));
}

String::String(double n, int digits) {
    init();
    char buf[34];
    snprintf(buf, sizeof(buf), "%.*f", digits, n);
    copy(buf, strlen(buf));
}

String::~String() { invalidate(); }

/* ----- Assignment ----- */
String &String::operator=(const String &rhs) {
    if (this == &rhs) return *this;
    copy(rhs._buf ? rhs._buf : "", rhs._len);
    return *this;
}
String &String::operator=(const char *cstr) {
    copy(cstr ? cstr : "", cstr ? strlen(cstr) : 0);
    return *this;
}

/* ----- Concatenation ----- */
bool String::concat(const char *cstr) {
    if (!cstr) return false;
    unsigned int sl = strlen(cstr);
    if (!reserve(_len + sl)) return false;
    memcpy(_buf + _len, cstr, sl);
    _len += sl;
    _buf[_len] = '\0';
    return true;
}
bool String::concat(const String &s) { return concat(s._buf ? s._buf : ""); }

String &String::operator+=(const String &rhs) { concat(rhs); return *this; }
String &String::operator+=(const char *cstr)  { concat(cstr); return *this; }
String &String::operator+=(char c) {
    char buf[2] = { c, '\0' };
    concat(buf);
    return *this;
}
String &String::operator+=(int n)            { String t(n);  concat(t); return *this; }
String &String::operator+=(unsigned int n)   { String t(n);  concat(t); return *this; }
String &String::operator+=(long n)           { String t(n);  concat(t); return *this; }
String &String::operator+=(unsigned long n)  { String t(n);  concat(t); return *this; }
String &String::operator+=(float n)          { String t(n);  concat(t); return *this; }
String &String::operator+=(double n)         { String t(n);  concat(t); return *this; }

/* ----- Comparison ----- */
bool String::operator==(const String &rhs) const {
    return strcmp(c_str(), rhs.c_str()) == 0;
}
bool String::operator==(const char *cstr) const {
    return strcmp(c_str(), cstr ? cstr : "") == 0;
}
bool String::operator<(const String &rhs) const {
    return strcmp(c_str(), rhs.c_str()) < 0;
}

/* ----- Indexing ----- */
char String::operator[](unsigned int i) const {
    if (i >= _len || !_buf) return '\0';
    return _buf[i];
}
char &String::operator[](unsigned int i) {
    static char dummy = '\0';
    if (i >= _len || !_buf) return dummy;
    return _buf[i];
}

/* ----- Manipulation ----- */
void String::toCharArray(char *buf, unsigned int bufsize) const {
    if (!buf || bufsize == 0) return;
    unsigned int n = (_len < bufsize - 1) ? _len : bufsize - 1;
    memcpy(buf, c_str(), n);
    buf[n] = '\0';
}

long String::toInt(void) const { return _buf ? strtol(_buf, NULL, 10) : 0; }
float String::toFloat(void) const { return _buf ? (float)strtod(_buf, NULL) : 0.0f; }
double String::toDouble(void) const { return _buf ? strtod(_buf, NULL) : 0.0; }

String String::substring(unsigned int from) const {
    return substring(from, _len);
}
String String::substring(unsigned int from, unsigned int to) const {
    if (from > _len) from = _len;
    if (to > _len) to = _len;
    if (from > to) { unsigned int t = from; from = to; to = t; }
    String result;
    result.copy(c_str() + from, to - from);
    return result;
}

int String::indexOf(char c) const {
    if (!_buf) return -1;
    const char *p = strchr(_buf, c);
    return p ? (int)(p - _buf) : -1;
}
int String::indexOf(char c, unsigned int fromIndex) const {
    if (!_buf || fromIndex >= _len) return -1;
    const char *p = strchr(_buf + fromIndex, c);
    return p ? (int)(p - _buf) : -1;
}
int String::indexOf(const char *str) const {
    if (!_buf || !str) return -1;
    const char *p = strstr(_buf, str);
    return p ? (int)(p - _buf) : -1;
}

void String::replace(const String &find, const String &repl)
{
    if (find.length() == 0 || !_buf) return;
    String result;
    unsigned int i = 0;
    while (i < _len) {
        if (i + find.length() <= _len &&
            memcmp(_buf + i, find.c_str(), find.length()) == 0) {
            result.concat(repl);
            i += find.length();
        } else {
            result += _buf[i];
            i++;
        }
    }
    *this = result;
}

void String::trim(void)
{
    if (!_buf || _len == 0) return;
    unsigned int start = 0, end = _len;
    while (start < end && isspace((unsigned char)_buf[start])) start++;
    while (end > start && isspace((unsigned char)_buf[end - 1])) end--;
    *this = substring(start, end);
}

void String::toUpperCase(void) {
    for (unsigned int i = 0; i < _len; i++) _buf[i] = toupper((unsigned char)_buf[i]);
}
void String::toLowerCase(void) {
    for (unsigned int i = 0; i < _len; i++) _buf[i] = tolower((unsigned char)_buf[i]);
}

bool String::startsWith(const String &prefix) const {
    if (prefix.length() > _len) return false;
    return memcmp(_buf, prefix.c_str(), prefix.length()) == 0;
}
bool String::endsWith(const String &suffix) const {
    if (suffix.length() > _len) return false;
    return memcmp(_buf + _len - suffix.length(), suffix.c_str(), suffix.length()) == 0;
}
bool String::equalsIgnoreCase(const String &s) const {
    if (_len != s._len) return false;
    for (unsigned int i = 0; i < _len; i++)
        if (tolower((unsigned char)_buf[i]) != tolower((unsigned char)s._buf[i]))
            return false;
    return true;
}
