// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Pieter te Brake

#include "transposition_table.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>



static const size_t TT_KEY_BYTES   = TT_ENTRY_COUNT * sizeof(TTKey);
static const size_t TT_SCORE_BYTES = TT_ENTRY_COUNT * sizeof(TTScore);


struct TranspositionTable new_tt(void) {
    struct TranspositionTable tt;

    tt.keys   = malloc(TT_KEY_BYTES);
    tt.scores = malloc(TT_SCORE_BYTES);

    return tt;
}

void reset_tt(struct TranspositionTable tt) {
    memset(tt.keys, 0, TT_KEY_BYTES);
}

void destroy_tt(struct TranspositionTable tt) {
    free(tt.keys);
    free(tt.scores);
}
