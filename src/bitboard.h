// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Pieter te Brake

#ifndef FOURSIGHT_BITBOARD_H_
#define FOURSIGHT_BITBOARD_H_


#include <assert.h>
#include <limits.h>
#include <stdbool.h>
#include <stdint.h>

#include "util.h"



enum Color {
    COLOR_RED    = 0,
    COLOR_YELLOW = 1,

    COLOR_COUNT
};
static_assert(COLOR_COUNT == 2, "There should be 2 colors");


enum Column {
    COLUMN_1 = 0,
    COLUMN_2,
    COLUMN_3,
    COLUMN_4,
    COLUMN_5,
    COLUMN_6,
    COLUMN_7,

    COLUMN_COUNT,

    COLUMN_HEIGHT = 7
};
static_assert(COLUMN_COUNT == 7, "There should be 7 columns");

// Returns whether `column` is valid.
static INLINE bool is_valid_column(const enum Column column) {
    return column >= COLUMN_1 && column < COLUMN_COUNT;
}


enum Row {
    ROW_1 = 0,
    ROW_2,
    ROW_3,
    ROW_4,
    ROW_5,
    ROW_6,

    ROW_COUNT
};
static_assert(ROW_COUNT == 6, "There should be 6 rows");

// Returns whether `row` is valid.
static INLINE bool is_valid_row(const enum Row row) {
    return row >= ROW_1 && row < ROW_COUNT;
}


static const unsigned BOARD_SIZE = (unsigned)COLUMN_COUNT * (unsigned)ROW_COUNT;


typedef uint64_t Bitboard;

static const Bitboard EMPTY_BITBOARD   = 0;
static const Bitboard INVALID_BITBOARD = UINT64_MAX;

// clang-format off
static const Bitboard BOTTOM_BITBOARDS[COLUMN_COUNT] = {
    (Bitboard)0x0000000000000001 << (COLUMN_HEIGHT * COLUMN_1),
    (Bitboard)0x0000000000000001 << (COLUMN_HEIGHT * COLUMN_2),
    (Bitboard)0x0000000000000001 << (COLUMN_HEIGHT * COLUMN_3),
    (Bitboard)0x0000000000000001 << (COLUMN_HEIGHT * COLUMN_4),
    (Bitboard)0x0000000000000001 << (COLUMN_HEIGHT * COLUMN_5),
    (Bitboard)0x0000000000000001 << (COLUMN_HEIGHT * COLUMN_6),
    (Bitboard)0x0000000000000001 << (COLUMN_HEIGHT * COLUMN_7),
};
static const Bitboard TOP_BITBOARDS[COLUMN_COUNT] = {
    (Bitboard)0x0000000000000001 << (COLUMN_HEIGHT * (COLUMN_1 + 1) - 2),
    (Bitboard)0x0000000000000001 << (COLUMN_HEIGHT * (COLUMN_2 + 1) - 2),
    (Bitboard)0x0000000000000001 << (COLUMN_HEIGHT * (COLUMN_3 + 1) - 2),
    (Bitboard)0x0000000000000001 << (COLUMN_HEIGHT * (COLUMN_4 + 1) - 2),
    (Bitboard)0x0000000000000001 << (COLUMN_HEIGHT * (COLUMN_5 + 1) - 2),
    (Bitboard)0x0000000000000001 << (COLUMN_HEIGHT * (COLUMN_6 + 1) - 2),
    (Bitboard)0x0000000000000001 << (COLUMN_HEIGHT * (COLUMN_7 + 1) - 2),
};
static const Bitboard COLUMN_BITBOARDS[COLUMN_COUNT] = {
    (Bitboard)0x000000000000003f << (COLUMN_HEIGHT * COLUMN_1),
    (Bitboard)0x000000000000003f << (COLUMN_HEIGHT * COLUMN_2),
    (Bitboard)0x000000000000003f << (COLUMN_HEIGHT * COLUMN_3),
    (Bitboard)0x000000000000003f << (COLUMN_HEIGHT * COLUMN_4),
    (Bitboard)0x000000000000003f << (COLUMN_HEIGHT * COLUMN_5),
    (Bitboard)0x000000000000003f << (COLUMN_HEIGHT * COLUMN_6),
    (Bitboard)0x000000000000003f << (COLUMN_HEIGHT * COLUMN_7),
};
// clang-format on

static const Bitboard BOTTOM_ROW_BITBOARD = 0x0000040810204081;
static const Bitboard BOARD_MASK          = (BOTTOM_ROW_BITBOARD << ROW_1) | (BOTTOM_ROW_BITBOARD << ROW_2)
                                 | (BOTTOM_ROW_BITBOARD << ROW_3) | (BOTTOM_ROW_BITBOARD << ROW_4)
                                 | (BOTTOM_ROW_BITBOARD << ROW_5) | (BOTTOM_ROW_BITBOARD << ROW_6);


// Returns a bitboard of the bottom cell of `column`.
static INLINE Bitboard bottom_bitboard(const enum Column column) {
    assert(is_valid_column(column));

    return BOTTOM_BITBOARDS[column];
}

// Returns a bitboard of the top cell of `column`.
static INLINE Bitboard top_bitboard(const enum Column column) {
    assert(is_valid_column(column));

    return TOP_BITBOARDS[column];
}

// Returns a bitboard of `column`.
static INLINE Bitboard column_bitboard(const enum Column column) {
    assert(is_valid_column(column));

    return COLUMN_BITBOARDS[column];
}


// Computes the column that `cell` is in, assuming cell lies on the board.
static INLINE enum Column column_of_cell(const Bitboard cell) {
    assert(cell != EMPTY_BITBOARD);

    const unsigned index = lsb(cell);

    return (enum Column)(index / COLUMN_HEIGHT);
}


// Prints `bitboard` in a human readable format to `stdout` for debugging purposes.
__attribute__((unused)) void print_bitboard(const Bitboard bitboard);



#endif  // FOURSIGHT_BITBOARD_H_
