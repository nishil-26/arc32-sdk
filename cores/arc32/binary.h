/*
 * binary.h  —  Arduino-compatible B-prefixed binary literal macros
 * (kept for sketch compatibility with classic Arduino cores)
 */
#ifndef BINARY_H
#define BINARY_H

#define B0  0
#define B1  1
#define B00 0
#define B01 1
#define B10 2
#define B11 3
/* Full 8-bit set is conventionally generated; modern GCC supports
   0b-prefixed literals natively, so sketches should prefer those.
   This header exists only for legacy Arduino library compatibility. */

#endif /* BINARY_H */
