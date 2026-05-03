// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Pieter te Brake

#include "bitboard.h"

#include <assert.h>
#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>

#include "util.h"



static Bitboard row_bitboard(const enum Row row) {
    assert(is_valid_row(row));

    return BOTTOM_ROW_BITBOARD << row;
}

static Bitboard bitboard_from_coordinates(const enum Column column, const enum Row row) {
    assert(is_valid_column(column));
    assert(is_valid_row(row));

    return column_bitboard(column) & row_bitboard(row);
}

__attribute__((unused)) void print_bitboard(const Bitboard bitboard) {
    puts("+---+---+---+---+---+---+---+");

    for (enum Row row = ROW_6;; --row) {
        for (enum Column column = COLUMN_1; column <= COLUMN_7; ++column)
            printf(((bitboard & bitboard_from_coordinates(column, row)) == EMPTY_BITBOARD) ? "|   " : "| X ");

        puts("|\n+---+---+---+---+---+---+---+");

        if (row == ROW_1)
            break;
    }

    static_assert(IS_SAME_TYPE(Bitboard, uint64_t), "wrong format specifier used");
    printf(
    "  1   2   3   4   5   6   7\n"
    "Hex: 0x%016" PRIx64 "\n",
    bitboard);
}
