// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Pieter te Brake

#ifndef FOURSIGHT_TRANSPOSITION_TABLE_H_
#define FOURSIGHT_TRANSPOSITION_TABLE_H_


#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

#include "position.h"
#include "util.h"



typedef uint32_t TTKey;
typedef uint8_t TTScore;

static const size_t TT_ENTRY_COUNT = ((size_t)1 << 23) + 9;  // prime number

struct TranspositionTable {
    TTKey* keys;
    TTScore* scores;
};


// Allocates a new transposition table with `TT_ENTRY_COUNT` entries.
struct TranspositionTable new_tt(void);

// Resets `tt` by zeroing all entries.
void reset_tt(struct TranspositionTable tt);

// Deallocates `tt`.
void destroy_tt(struct TranspositionTable tt);


// Returns the index that belongs to `key` in the transposition table.
static INLINE size_t tt_index(const Key key) {
    assert(key <= MAX_KEY);

    return key % TT_ENTRY_COUNT;
}

static INLINE TTScore tt_probe(const struct TranspositionTable tt, const Key key) {
    assert(key <= MAX_KEY);

    const size_t index = tt_index(key);

    if (tt.keys[index] == (TTKey)key)
        return tt.scores[index];

    return 0;
}

static INLINE void tt_store(struct TranspositionTable tt, const Key key, const TTScore score) {
    assert(key <= MAX_KEY);

    const size_t index = tt_index(key);

    tt.keys[index]   = (TTKey)key;
    tt.scores[index] = score;
}



#endif  // #ifndef FOURSIGHT_TRANSPOSITION_TABLE_H_
