// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Pieter te Brake

#ifndef FOURSIGHT_UTIL_H_
#define FOURSIGHT_UTIL_H_


#include <assert.h>
#include <stdint.h>



#define INLINE               inline __attribute__((always_inline))
#define IS_SAME_TYPE(T1, T2) _Generic((T1){0}, T2: true, default: false)


#ifndef __has_builtin
#    error "__has_builtin macro is required."
#endif  // #ifndef __has_builtin


// Returns the index of the least significant bit of `x`, assuming
// it is nonzero.
static INLINE unsigned lsb(const uint64_t x) {
    assert(x != 0);

#if __has_builtin(__builtin_ctzll)

    return (unsigned)__builtin_ctzll(x);

#else

    // Fallback.
    // clang-format off
    static const unsigned index64[64] = {
         0,  1, 48,  2, 57, 49, 28,  3,
        61, 58, 50, 42, 38, 29, 17,  4,
        62, 55, 59, 36, 53, 51, 43, 22,
        45, 39, 33, 30, 24, 18, 12,  5,
        63, 47, 56, 27, 60, 41, 37, 16,
        54, 35, 52, 21, 44, 32, 23, 11,
        46, 26, 40, 15, 34, 20, 31, 10,
        25, 14, 19,  9, 13,  8,  7,  6
    };
    // clang-format on
    return index64[((x & -x) * 0x03f79d71b4cb0a89ULL) >> 58];

#endif  // #if __has_builtin(__builtin_ctzll)
}

// Returns the number of 1-bits of `x`.
static INLINE unsigned popcount(uint64_t x) {
#if __has_builtin(__builtin_popcountll)

    return (unsigned)__builtin_popcountll(x);

#else

    // Fallback.
    x *= 0x0002000400080010ULL;
    x &= 0x1111111111111111ULL;
    x *= 0x1111111111111111ULL;
    x >>= 60;
    return (unsigned)x;

#endif  // #if __has_builtin(__builtin_popcountll)
}



#endif  // #ifndef FOURSIGHT_UTIL_H_
