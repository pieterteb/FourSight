// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Pieter te Brake

#ifndef FOURSIGHT_SIMD_H_
#define FOURSIGHT_SIMD_H_


#include <stdint.h>



#if defined(__AVX512F__)

#    include <immintrin.h>
#    define HAS_AVX512 1

typedef __m512i vec_512;
typedef __m512i vec_64x8;
typedef __m512i vec_8x64;

#    define vec_setzero_512() _mm512_setzero_si512()
#    define vec_and_512(a, b) _mm512_and_si512(a, b)

#    define vec_loadu_64x8(mem_addr) _mm512_loadu_epi8(mem_addr)
#    define vec_add_64x8(a, b)       _mm512_add_epi8(a, b)
#    define vec_shuffle_64x8(a, b)   _mm512_shuffle_epi8(a, b)
#    define vec_sad_64x8(a, b)       _mm512_sad_epu8(a, b)

#    define vec_loadu_8x64(mem_addr)             _mm512_loadu_epi64(mem_addr)
#    define vec_storeu_8x64(mem_addr, a)         _mm512_storeu_epi64(mem_addr, a)
#    define vec_set1_8x64(a)                     _mm512_set1_epi64(a)
#    define vec_or_8x64(a, b)                    _mm512_or_epi64(a, b)
#    define vec_and_8x64(a, b)                   _mm512_and_epi64(a, b)
#    define vec_slli_8x64(a, imm8)               _mm512_slli_epi64(a, imm8)
#    define vec_srli_8x64(a, imm8)               _mm512_srli_epi64(a, imm8)
#    define vec_ternarylogic_8x64(a, b, c, imm8) _mm512_ternarylogic_epi64(a, b, c, imm8)
#    define vec_add_8x64(a, b)                   _mm512_add_epi64(a, b)
#    define vec_sub_8x64(a, b)                   _mm512_sub_epi64(a, b)
#    define vec_mullo_8x64(a, b)                 _mm512_mullo_epi64(a, b)

#    if defined(__AVX512VPOPCNTDQ__)

#        define vec_popcnt_8x64(a) _mm512_popcnt_epi64(a)

#    else

#        include "util.h"

static INLINE vec_popcnt_8x64(vec_8x64 a) {
#        if defined(__AVX512BW__)

    // clang-format off
    static const uint8_t lut_bytes[64] = {
        0,1,1,2,1,2,2,3,1,2,2,3,2,3,3,4,
        0,1,1,2,1,2,2,3,1,2,2,3,2,3,3,4,
        0,1,1,2,1,2,2,3,1,2,2,3,2,3,3,4,
        0,1,1,2,1,2,2,3,1,2,2,3,2,3,3,4
    };
    // clang-format on

    const vec_64x8 lut      = vec_loadu_64x8(lut_butes);
    const vec_64x8 low_mask = vec_set1_64x8(0x0F);


    // Split nibbles.
    const vec_64x8 low  = (vec_64x8)vec_and_512((vec_512)a, (vec_512)low_mask);
    const vec_64x8 high = (vec_64x8)vec_and_512((vec_512)vec_srli_8x64(a, 4), (vec_512)low_mask);

    // Lookup popcount per nibble.
    const vec_64x8 count_low  = vec_shuffle_64x8(lut, low);
    const vec_64x8 count_high = vec_shuffle_64x8(lut, hight);

    // Per-byte popcount.
    const vec_64x8 count_bytes = vec_add_64x8(count_low, count_high);

    // Sum bytes within each 64-bt lane.
    return vec_sad_64x8(count_bytes, vec_setzero_512());

#        else

    const vec_512 mask1           = (vec_512)vec_set1_8x64((uint64_t)0x5555555555555555);
    const vec_512 mask2           = (vec_512)vec_set1_8x64((uint64_t)0x3333333333333333);
    const vec_512 mask4           = (vec_512)vec_set1_8x64((uint64_t)0x0F0F0F0F0F0F0F0F);
    const vec_8x64 horizontal_mul = vec_set1_8x64((uint64_t)0x0101010101010101);

    // Count bits per 2-bit group.
    a = vec_sub_8x64(a, (vec_8x64)vec_and_512((vec_512)vec_srli_8x64(a, 1), mask1));

    // Sum 2-bit groups into nibbles.
    a = vec_add_8x64((vec_8x64)vec_and_512((vec_512)a, mask2),
                     (vec_8x64)vec_and_512((vec_512)vec_srli_8x64(a, 2), mask2));

    // Sum nibbles into bytes.
    a = (vec_8x64)vec_and_512((vec_512)vec_add_8x64(a, vec_srli_8x64(a, 4)), mask4);

#            if defined(__AVX512DQ__)

    // Horizontal multiply to accumulate byte counts into most significant byte.
    a = vec_mullo_8x64(a, horizontal_mul);

#            else

    // Accumulate byte counts into least significant byte.
    a = vec_add_8x64(a, vec_srli_8x64(a, 8));
    a = vec_add_8x64(a, vec_srli_8x64(a, 16));
    a = vec_add_8x64(a, vec_srli_8x64(a, 32));

#            endif  // #if defined(__AVX512DQ__)

    // Shift to extract the most significant byte.
    return vec_srli_8x64(a, 64 - 8);

#        endif  // #if defined(__AVX512BW__)
}

#    endif  // #if defined(__AVX512VPOPCNTDQ__)

#else

#    define HAS_AVX512 0

#endif  // #if defined(__AVX512__)



#endif  // #ifndef FOURSIGHT_SIMD_H_
