// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Pieter te Brake

#ifndef FOURSIGHT_POSITION_H_
#define FOURSIGHT_POSITION_H_


#include <assert.h>
#include <stdbool.h>
#include <stddef.h>

#include "bitboard.h"
#include "util.h"



// The board occupation of a color will be represented by a 64-bit integer.
// Each column will consist of 7 bits. This allows us to efficiently execute
// moves and detect when the game is over. The actual occupation will only
// use the first 6 bits of each column. We default the other bits to 0.
// See layout below:
//
//      .  .  .  .  .  .  .   <- sentinel row
//      5 12 19 26 33 40 47
//      4 11 18 25 32 39 46
//      3 10 17 24 31 38 45
//      2  9 16 23 30 37 44
//      1  8 15 22 29 36 43
//      0  7 14 21 28 35 42
//
// The position struct consists of two bitboards. `current` contains
// the occupancy of the player that is to move and `total` contains
// the total occupancy of the board.
struct Position {
    Bitboard current;
    Bitboard total;
};

static const struct Position EMPTY_POSITION   = {0};
static const struct Position INVALID_POSITION = {INVALID_BITBOARD, INVALID_BITBOARD};


typedef Bitboard Key;

static const Key MAX_KEY = ((Key)1 << (COLUMN_COUNT * COLUMN_HEIGHT + 1)) - 1;


// Returns the number of moves played in `postion`.
static INLINE unsigned moves_played(const struct Position position) {
    return popcount(position.total);
}

// Returns the color of side to move in `position`.
static INLINE enum Color side_to_move(const struct Position position) {
    return (enum Color)(moves_played(position) & 1);  // Fast modulo 2.
}

// Returns a key uniquely identifying `position`.
static INLINE Key position_key(const struct Position position) {
    return (Key)(position.current + position.total);
}


// Computes a bitboard of all possible moves in `position`.
static INLINE Bitboard possible_moves(const struct Position position) {
    return (position.total + BOTTOM_ROW_BITBOARD) & BOARD_MASK;
}

// Computes a bitboard of all connect 4 threats of `occupancy`. The threats may
// overlap with cells that are already occupied or lie in a sentinel cell. A cell
// is a threat if it forms a connect 4 when filled.
static INLINE Bitboard compute_threats(const Bitboard occupancy) {
    // Vertical.
    unsigned shift         = 1;
    Bitboard shifted_left1 = occupancy << 1;
    Bitboard shifted_left2 = occupancy << 2;
    Bitboard shifted_left3 = occupancy << 3;
    Bitboard threats       = shifted_left1 & shifted_left2 & shifted_left3;

    // Horizontal.
    shift                   = COLUMN_HEIGHT;
    shifted_left1           = occupancy << shift;
    shifted_left2           = occupancy << (2 * shift);
    shifted_left3           = occupancy << (3 * shift);
    Bitboard shifted_right1 = occupancy >> shift;
    Bitboard shifted_right3 = occupancy >> (3 * shift);
    Bitboard mask           = shifted_left1 & shifted_left2;
    Bitboard threat12       = mask & (shifted_left3 | shifted_right1);
    mask >>= 3 * shift;
    Bitboard threat34 = mask & (shifted_left1 | shifted_right3);
    threats |= threat12 | threat34;

    // Diagonal /.
    shift          = COLUMN_HEIGHT + 1;
    shifted_left1  = occupancy << shift;
    shifted_left2  = occupancy << (2 * shift);
    shifted_left3  = occupancy << (3 * shift);
    shifted_right1 = occupancy >> shift;
    shifted_right3 = occupancy >> (3 * shift);
    mask           = shifted_left1 & shifted_left2;
    threat12       = mask & (shifted_left3 | shifted_right1);
    mask >>= 3 * shift;
    threat34 = mask & (shifted_left1 | shifted_right3);
    threats |= threat12 | threat34;

    // Anti-diagonal \.
    shift          = COLUMN_HEIGHT - 1;
    shifted_left1  = occupancy << shift;
    shifted_left2  = occupancy << (2 * shift);
    shifted_left3  = occupancy << (3 * shift);
    shifted_right1 = occupancy >> shift;
    shifted_right3 = occupancy >> (3 * shift);
    mask           = shifted_left1 & shifted_left2;
    threat12       = mask & (shifted_left3 | shifted_right1);
    mask >>= 3 * shift;
    threat34 = mask & (shifted_left1 | shifted_right3);
    threats |= threat12 | threat34;

    return threats & BOARD_MASK;
}

// Computes a bitboard of moves that do not immediately lose in `position`.
// This function assumes that the current player does not have an immediate
// winning move.
static INLINE Bitboard non_losing_moves(const struct Position position) {
    Bitboard possible      = possible_moves(position);
    const Bitboard threats = compute_threats(position.current ^ position.total);
    const Bitboard forced  = possible & threats;


    if ((forced & (forced - 1)) != EMPTY_BITBOARD)
        return EMPTY_BITBOARD;  // Everything loses if there are multiple forced moves.

    if (forced != EMPTY_BITBOARD)
        possible = forced;

    // Playing below a threat loses the game.
    return possible & ~(threats >> 1);
}


// Returns `true` if `column` is not full in `position`.
static INLINE bool can_play_column(const struct Position position, const enum Column column) {
    assert(is_valid_column(column));

    // The token can be played if the top cell is empty.
    return (position.total & top_bitboard(column)) == EMPTY_BITBOARD;
}

// Plays `move` in `position` assuming it is legal.
static INLINE struct Position play(struct Position position, const Bitboard move) {
    position.current ^= position.total;
    position.total |= move;

    return position;
}

// Plays a token in `position` at `column`, assuming it is legal.
static INLINE struct Position play_column(struct Position position, const enum Column column) {
    assert(is_valid_column(column));
    assert(can_play_column(position, column));

    position.current ^= position.total;
    position.total |= position.total + bottom_bitboard(column);

    return position;
}


// Returns `true` if there is a connect four in `occupancy`, else returns `false`.
static INLINE bool connect_four(const Bitboard occupancy) {
    // Horizontal.
    const unsigned horizontal_shift = COLUMN_HEIGHT;
    Bitboard mask                   = occupancy & (occupancy >> horizontal_shift);
    if ((mask & (mask >> (2 * horizontal_shift))) != EMPTY_BITBOARD)
        return true;

    // Diagonal /.
    const unsigned diagonal_shift = COLUMN_HEIGHT + 1;
    mask                          = occupancy & (occupancy >> diagonal_shift);
    if ((mask & (mask >> (2 * diagonal_shift))) != EMPTY_BITBOARD)
        return true;

    // Anti-diagonal \.
    const unsigned antidiagonal_shift = COLUMN_HEIGHT - 1;
    mask                              = occupancy & (occupancy >> antidiagonal_shift);
    if ((mask & (mask >> (2 * antidiagonal_shift))) != EMPTY_BITBOARD)
        return true;

    // Vertical.
    mask = occupancy & (occupancy >> 1);
    return (mask & (mask >> 2)) != EMPTY_BITBOARD;
}

// Returns `true` if playing a token in `column` wins the game, else returns `false`.
// This function assumes `column` is playable.
static INLINE bool is_winning_move(const struct Position position, const enum Column column) {
    assert(is_valid_column(column));
    assert(can_play_column(position, column));

    const Bitboard occupancy = position.current | ((position.total + BOTTOM_ROW_BITBOARD) & column_bitboard(column));

    return connect_four(occupancy);
}


// Prints `position` to `stdout` for debugging purposes.
void print_position(const struct Position position);



#endif  // #ifndef FOURSIGHT_POSITION_H_
