// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Pieter te Brake

#include "move_ordering.h"

#include <stdlib.h>

#include "bitboard.h"
#include "position.h"



// Computes the score of `move` in `position`.
static INLINE MoveScore move_score(const struct Position position, const Bitboard move) {
    assert(is_valid_column(column_of_cell(move)));

    return popcount(compute_threats(position.current | move) & ~position.total);
}

// Adds `move` with `score` to `order` such that `order` remains sorted according
// to move scores.
static void add_move(struct MoveOrder* restrict order, const Bitboard move, const MoveScore score) {
    assert(order != NULL);

    size_t i = order->move_count++;

    for (; i != 0 && order->entries[i - 1].score > score; --i)
        order->entries[i] = order->entries[i - 1];

    order->entries[i].move  = move;
    order->entries[i].score = score;
}

void compute_move_order(struct MoveOrder* restrict order, const struct Position position) {
    assert(order != NULL);

    // Static reversed order in which to search columns. From middle to sides.
    static const enum Column reverse_column_order[COLUMN_COUNT] = {COLUMN_1, COLUMN_7, COLUMN_2, COLUMN_6,
                                                                   COLUMN_3, COLUMN_5, COLUMN_4};

    order->move_count = 0;

    Bitboard moves = non_losing_moves(position);
    unsigned i     = 0;

    while (moves != EMPTY_BITBOARD && i < COLUMN_COUNT) {
        const Bitboard column = column_bitboard(reverse_column_order[i++]);
        const Bitboard move   = moves & column;

        if (move != EMPTY_BITBOARD) {
            add_move(order, move, move_score(position, move));
            moves &= ~column;
        }
    }
}
