// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Pieter te Brake

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "benchmark.h"
#include "bitboard.h"
#include "position.h"
#include "search.h"
#include "transposition_table.h"



int main(int argc, const char* argv[]) {
    if (argc > 3 || (argc == 2 && (strcmp(argv[1], "bench") == 0 || strcmp(argv[1], "help") == 0))) {
        // Incorrect usage or help

        fputs("Usage: ./foursight [help | <position> | bench <file_count>]\n", stderr);
    } else if (argc > 1 && strcmp(argv[1], "bench") == 0) {
        // Benchmarking

        const unsigned file_count = (unsigned)strtoul(argv[2], NULL, 10);

        if (file_count > TEST_FILE_COUNT) {
            fprintf(stderr, "'file_count' must be less than or equal to %zu\n", TEST_FILE_COUNT);
            return 0;
        }

        benchmark(file_count);
    } else {
        // Solve position

        const struct Position root_position = parse_position((argc == 1) ? NULL : argv[1]);

        if (root_position.current != INVALID_BITBOARD) {
            print_position(root_position);
            printf("To move:    %s\n", (side_to_move(root_position) == COLOR_RED) ? "Red" : "Yellow");
            putchar('\n');

            struct TranspositionTable tt = new_tt();
            reset_tt(tt);

            enum Column best_move = COLUMN_1;
            size_t time_ns        = 0;
            size_t node_count     = 0;
            const Score score     = solve(root_position, tt, &best_move, &time_ns, &node_count);

            destroy_tt(tt);

            const double time_s = (double)time_ns / 1000000000.0;

            printf(
            "Best move:         %u\n"
            "Score:             %d\n"
            "Time:              %.6f s\n"
            "Nodes searched:    %zu\n"
            "Nodes per second:  %.2f kN/s\n",
            best_move + 1, score, time_s, node_count, (double)node_count / time_s / 1000.0);
        }
    }

    return 0;
}
