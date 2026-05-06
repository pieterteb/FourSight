// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Pieter te Brake

#ifndef FOURSIGHT_MOVE_ORDERING_H_
#define FOURSIGHT_MOVE_ORDERING_H_


#include <stddef.h>

#include "bitboard.h"
#include "position.h"
#include "simd.h"
#include "util.h"



#if HAS_AVX512

typedef Bitboard MoveScore;

#else

typedef unsigned MoveScore;

#endif  // #if HAS_AVX512


struct MoveOrder {
    Bitboard moves[COLUMN_COUNT];
    MoveScore scores[COLUMN_COUNT];

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
        return order->moves[--order->move_count];

    return EMPTY_BITBOARD;
}



#endif  // #ifndef FOURSIGHT_MOVE_ORDERING_H_
