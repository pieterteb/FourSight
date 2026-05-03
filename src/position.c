// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Pieter te Brake

#include "position.h"

#include <assert.h>
#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>

#include "bitboard.h"
#include "util.h"



void print_position(const struct Position position) {
    const enum Color to_move = side_to_move(position);
    const char current_char  = (to_move == COLOR_RED) ? 'R' : 'Y';
    const char opponent_char = (to_move == COLOR_RED) ? 'Y' : 'R';

    puts("+---+---+---+---+---+---+---+");

    const Bitboard current  = position.current;
    const Bitboard opponent = position.current ^ position.total;

    for (enum Row row = ROW_6;; --row) {
        for (enum Column column = COLUMN_1; column <= COLUMN_7; ++column) {
            const int bit_index = (int)column * COLUMN_HEIGHT + (int)row;
            const Bitboard mask = (Bitboard)1 << bit_index;

            const char cell = ((current & mask) != EMPTY_BITBOARD)  ? current_char
                            : ((opponent & mask) != EMPTY_BITBOARD) ? opponent_char
                                                                    : ' ';

            printf("| %c ", cell);
        }

        puts("|\n+---+---+---+---+---+---+---+");

        if (row == ROW_1)
            break;
    }

    static_assert(IS_SAME_TYPE(Bitboard, uint64_t), "Wrong format specifier used");
    puts("  1   2   3   4   5   6   7");
}
