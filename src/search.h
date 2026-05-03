// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Pieter te Brake

#ifndef FOURSIGHT_SEARCH_H_
#define FOURSIGHT_SEARCH_H_


#include <stddef.h>

#include "bitboard.h"
#include "position.h"
#include "transposition_table.h"



// A positive score indicates a winning position and a negative score
// a losing position. The magnitude of the score is one plus the number
// of tokens that remain unplayed for the winning player with perfect
// play. So, a game won with the last token has score 1, second to last
// 2, etc. The score is 0 for a drawn game.
//
// In case of a weak solution, the magnitude of the scores is 1 if the
// result is decisive and 0 in case of a draw.

typedef int Score;

static const Score DRAW_SCORE = 0;
static const Score MIN_SCORE  = -((Score)BOARD_SIZE / 2 + 1 - 4);
static const Score MAX_SCORE  = ((Score)BOARD_SIZE + 1) / 2 + 1 - 4;


static const unsigned MAX_GAME_LENGTH = BOARD_SIZE;


// Solves `position` and returns its score. Also updates `best_move`,
// `time_ns` and `node_count`, representing the best move, time taken
// in nanoseconds and positions searched respectively.
Score solve(const struct Position position, struct TranspositionTable tt, enum Column* restrict best_move,
            size_t* restrict time_ns, size_t* restrict node_count);



#endif  // #ifndef FOURSIGHT_SEARCH_H_
