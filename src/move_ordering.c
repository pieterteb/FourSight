// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Pieter te Brake

#include "move_ordering.h"

#include <stdint.h>
#include <stdlib.h>

#include "bitboard.h"
#include "position.h"
#include "simd.h"
#include "util.h"



#if HAS_AVX512

static INLINE vec_8x64 move_score(const struct Position position, vec_8x64 moves) {
    vec_8x64 threats = compute_threats_vector(vec_or_8x64(moves, vec_set1_8x64((int64_t)position.current)));
    threats          = vec_and_8x64(threats, vec_set1_8x64((int64_t)~position.total));

    return vec_popcnt_8x64(threats);
}

#else

// Computes the score of `move` in `position`.
static INLINE MoveScore move_score(const struct Position position, const Bitboard move) {
    return popcount(compute_threats(position.current | move) & ~position.total);
}

#endif  // #if HAS_AVX512


// Adds `move` with `score` to `order` such that `order` remains sorted according
// to move scores.
static void add_move(struct MoveOrder* restrict order, const Bitboard move, const MoveScore score) {
    assert(order != NULL);

    size_t i = order->move_count++;

    for (; i > 0 && order->scores[i - 1] > score; --i) {
        order->scores[i] = order->scores[i - 1];
        order->moves[i]  = order->moves[i - 1];
    }

    order->moves[i]  = move;
    order->scores[i] = score;
}

void compute_move_order(struct MoveOrder* restrict order, const struct Position position) {
    assert(order != NULL);

    order->move_count = 0;

    const Bitboard moves = non_losing_moves(position);

#if HAS_AVX512

    if (moves == EMPTY_BITBOARD)
        return;

    // Static reversed order of the order in which to search columns: from middle to sides.
    static const Bitboard reverse_column_order[COLUMN_COUNT + 1] = {
    COLUMN_BITBOARDS[COLUMN_1], COLUMN_BITBOARDS[COLUMN_7], COLUMN_BITBOARDS[COLUMN_2], COLUMN_BITBOARDS[COLUMN_6],
    COLUMN_BITBOARDS[COLUMN_3], COLUMN_BITBOARDS[COLUMN_5], COLUMN_BITBOARDS[COLUMN_4], EMPTY_BITBOARD};

    const vec_8x64 columns   = vec_loadu_8x64(reverse_column_order);
    const vec_8x64 vec_moves = vec_and_8x64(vec_set1_8x64((int64_t)moves), columns);
    const vec_8x64 scores    = move_score(position, vec_moves);

    Bitboard temp_moves[COLUMN_COUNT + 1];
    Bitboard temp_scores[COLUMN_COUNT + 1];

    vec_storeu_8x64(temp_moves, vec_moves);
    vec_storeu_8x64(temp_scores, scores);

    for (unsigned i = 0; i < COLUMN_COUNT; ++i)
        if (temp_moves[i] != EMPTY_BITBOARD)
            add_move(order, temp_moves[i], temp_scores[i]);

#else

    // Static reversed order in which to search columns. From middle to sides.
    static const enum Column reverse_column_order[COLUMN_COUNT] = {COLUMN_1, COLUMN_7, COLUMN_2, COLUMN_6,
                                                                   COLUMN_3, COLUMN_5, COLUMN_4};

    for (unsigned i = 0; i < COLUMN_COUNT; ++i) {
        const Bitboard column = column_bitboard(reverse_column_order[i]);
        const Bitboard move   = moves & column;

        if (move != EMPTY_BITBOARD)
            add_move(order, move, move_score(position, move));
    }

#endif  // #if HAS_AVX512
}
