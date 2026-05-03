// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Pieter te Brake

#ifndef FOURSIGHT_MOVE_ORDERING_H_
#define FOURSIGHT_MOVE_ORDERING_H_


#include <stddef.h>

#include "bitboard.h"
#include "position.h"
#include "util.h"



typedef unsigned MoveScore;


struct MoveOrder {
    struct {
        Bitboard move;
        MoveScore score;
    } entries[COLUMN_COUNT];

    size_t move_count;
};


// Computes the scores of the relevant moves in `position` and orders them into
// `order` accordingly. Relevant moves are moves that do not immediately lose,
// assuming that there is no immediate win available.
void compute_move_order(struct MoveOrder* restrict order, const struct Position position);

// Returns a bitboard of the next move from `order` if it exists, else returns
// an empty bitboard.
static INLINE Bitboard next_move(struct MoveOrder* restrict order) {
    if (order->move_count != 0)
        return order->entries[--order->move_count].move;

    return EMPTY_BITBOARD;
}



#endif  // #ifndef FOURSIGHT_MOVE_ORDERING_H_
