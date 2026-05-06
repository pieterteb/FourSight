// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Pieter te Brake

#include "search.h"

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "bitboard.h"
#include "move_ordering.h"
#include "position.h"



// Returns the score belonging to a win on the next move if `moves_played`
// moves have already been played.
static INLINE Score win_score(const unsigned moves_played) {
    assert(moves_played < MAX_GAME_LENGTH);

    return (Score)((BOARD_SIZE + 1 - moves_played) / 2);
}


// Returns the score of `position` as specified in search.h if it lies between
// `alpha` and `beta`, else returns either an upper or lower bound for the score.
// Also updates `node_count` with the amount of positions searched.
static Score search(const struct Position position, struct TranspositionTable tt, const unsigned moves_played,
                    Score alpha, Score beta, size_t* restrict node_count) {
    assert(position.current != INVALID_BITBOARD && position.total != INVALID_BITBOARD);
    assert(node_count != NULL);
    assert(alpha < beta);

    ++*node_count;

    // If the max game length is reached, we have a draw since we checked that
    // the last move played was not a winning move.
    if (moves_played == MAX_GAME_LENGTH)
        return DRAW_SCORE;

    Score upper_bound;

    const TTScore tt_upper_bound = tt_probe(tt, position_key(position));
    if (tt_upper_bound != 0)
        upper_bound = (Score)tt_upper_bound + MIN_SCORE - 1;
    else  // We assure that no immediate win is available, so the win score becomes an upper bound.
        upper_bound = win_score(moves_played) - 1;

    if (beta > upper_bound) {
        beta = upper_bound;

        if (alpha >= beta)
            return beta;
    }

    struct MoveOrder order;
    compute_move_order(&order, position);

    Bitboard move = next_move(&order);
    while (move != EMPTY_BITBOARD) {
        const struct Position new_position = play(position, move);
        const Score score                  = -search(new_position, tt, moves_played + 1, -beta, -alpha, node_count);

        if (score >= beta)
            return score;

        if (score > alpha)
            alpha = score;

        move = next_move(&order);
    }

    // At this point, alpha is the best score we can obtain, hence it is the
    // upper bound for this position. We store it as such. We do not store
    // alpha exactly as it may be 0.
    tt_store(tt, position_key(position), (TTScore)(alpha - MIN_SCORE + 1));

    return alpha;
}

// Same as `search()` but also updates `best_move` with the best move.
static Score root_search(const struct Position position, struct TranspositionTable tt, const unsigned moves_played,
                         Score alpha, Score beta, enum Column* restrict best_move, size_t* restrict node_count) {
    assert(position.current != INVALID_BITBOARD && position.total != INVALID_BITBOARD);
    assert(best_move != NULL);
    assert(node_count != NULL);
    assert(alpha < beta);

    ++*node_count;

    // If the board is full, we have a draw since we checked that the last move
    // played was not a winning move.
    if (moves_played == MAX_GAME_LENGTH)
        return DRAW_SCORE;

    const Score win = win_score(moves_played);

    for (enum Column column = 0; column < COLUMN_COUNT; ++column) {
        if (can_play_column(position, column)) {
            *best_move = column;

            if (is_winning_move(position, column))
                return win;
        }
    }

    const Score upper_bound = win - 1;

    // The win score is an upper bound as there is no move that wins immediately.
    if (beta > upper_bound) {
        beta = upper_bound;

        if (alpha >= beta)
            return beta;
    }



    struct MoveOrder order;
    compute_move_order(&order, position);

    Bitboard move = next_move(&order);
    while (move != EMPTY_BITBOARD) {
        const struct Position new_position = play(position, move);
        const Score score                  = -search(new_position, tt, moves_played + 1, -beta, -alpha, node_count);

        if (score >= beta) {
            *best_move = column_of_cell(move);
            return score;
        }

        if (score > alpha) {
            alpha      = score;
            *best_move = column_of_cell(move);
        }

        move = next_move(&order);
    }

    return alpha;
}

Score solve(const struct Position position, struct TranspositionTable tt, enum Column* restrict best_move,
            size_t* restrict time_ns, size_t* restrict node_count) {
    assert(position.current != INVALID_BITBOARD && position.total != INVALID_BITBOARD);
    assert(best_move != NULL);
    assert(time_ns != NULL);
    assert(node_count != NULL);

    struct timespec start, end;

    clock_gettime(CLOCK_MONOTONIC, &start);

    const unsigned moves = moves_played(position);

    Score max = win_score(moves);
    Score min = -max;



    while (min < max) {
        Score mid = min + (max - min) / 2;

        // We bias our search away from 0 as we expect most scores to be around
        // 0. We will get quick fails restricting our bounds.
        if (mid <= 0 && min / 2 < mid)
            mid = min / 2;
        else if (mid >= 0 && max / 2 > mid)
            mid = max / 2;

        Score score = root_search(position, tt, moves, mid, mid + 1, best_move, node_count);

        if (score <= mid)
            max = score;
        else
            min = score;
    }

    clock_gettime(CLOCK_MONOTONIC, &end);

    *time_ns = (size_t)(end.tv_sec - start.tv_sec) * 1000000000 + (size_t)(end.tv_nsec - start.tv_nsec);

    return min;
}
