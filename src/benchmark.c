// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Pieter te Brake

#include "benchmark.h"

#include <assert.h>
#include <stddef.h>
#include <stdio.h>

#include "bitboard.h"
#include "position.h"
#include "search.h"
#include "transposition_table.h"



struct Position parse_position(const char* position_string) {
    struct Position position = EMPTY_POSITION;

    if (position_string == NULL)
        return position;

    const char* column_char = position_string;
    size_t tokens_played    = 0;

    while (*column_char != 0) {
        const enum Column column = (enum Column)(*column_char - '1');
        ++tokens_played;

        if (!is_valid_column(column)) {
            fprintf(stderr, "Invalid position: unknown column '%c' at move %zu\n", *column_char, tokens_played);
            return INVALID_POSITION;
        }

        if (!can_play_column(position, column)) {
            fprintf(stderr, "Invalid position: overfull column %u at move %zu\n", column + 1, tokens_played);
            return INVALID_POSITION;
        }

        if (is_winning_move(position, column)) {
            fprintf(stderr, "Invalid position: win for %s at move %zu\n",
                    (side_to_move(position) == COLOR_RED) ? "yellow" : "red", tokens_played);
            return INVALID_POSITION;
        }

        position = play_column(position, column);

        if (tokens_played == (size_t)BOARD_SIZE) {
            fprintf(stderr, "Invalid position: drawn position at move %zu\n", tokens_played);
            return INVALID_POSITION;
        }

        ++column_char;
    }

    return position;
}


void benchmark(const unsigned file_count) {
    assert(file_count <= 6);

    if (file_count == 0)
        return;

    // clang-format off
    const char test_files[][128] = {
        "../tests/Test_L3_R1",
        "../tests/Test_L2_R1",
        "../tests/Test_L2_R2",
        "../tests/Test_L1_R1",
        "../tests/Test_L1_R2",
        "../tests/Test_L1_R3",
    };
    // clang-format on

    const size_t POSITION_COUNT = 1000;

    printf("Benchmarking %u/%zu files...\n", file_count, TEST_FILE_COUNT);

    struct TranspositionTable tt = new_tt();
    reset_tt(tt);

    for (size_t i = 0; i < file_count; ++i) {
        const char* file_name = test_files[i];

        printf("\nRunning benchmark on %s...\n", file_name);

        FILE* test_file = fopen(file_name, "r");

        char test_positions[POSITION_COUNT][MAX_GAME_LENGTH];
        Score test_answers[POSITION_COUNT];

        for (size_t j = 0; j < POSITION_COUNT; ++j) {
            assert(MAX_GAME_LENGTH - 1 == 41);
            fscanf(test_file, "%41s %d", test_positions[j], &test_answers[j]);
        }

        size_t passed_tests = 0;
        size_t times_ns[POSITION_COUNT];
        size_t node_counts[POSITION_COUNT];

        size_t failed_test = (size_t)-1;
        Score failed_test_answer;
        Score failed_test_score;

        for (size_t j = 0; j < POSITION_COUNT; ++j) {
            const Score answer             = test_answers[j];
            const char* test_position      = test_positions[j];
            const struct Position position = parse_position(test_position);

            assert(position.current != INVALID_BITBOARD);

            printf("\rSolving position %4zu/%zu", j + 1, POSITION_COUNT);
            fflush(stdout);

            enum Column temp;
            times_ns[j]       = 0;
            node_counts[j]    = 0;
            const Score score = solve(position, tt, &temp, &times_ns[j], &node_counts[j]);

            reset_tt(tt);

            if (score == answer) {
                ++passed_tests;
            } else if (failed_test == (size_t)-1) {
                failed_test        = j;
                failed_test_answer = answer;
                failed_test_score  = score;
            }
        }
        putchar('\n');

        size_t total_time             = 0;
        size_t total_node_count       = 0;
        double total_nodes_per_second = 0;

        for (size_t j = 0; j < POSITION_COUNT; ++j) {
            total_time += times_ns[j];
            total_node_count += node_counts[j];
            total_nodes_per_second += (double)node_counts[j] / ((double)times_ns[j] / 1000000000.0);
        }

        double mean_time             = (double)total_time / (double)POSITION_COUNT;
        double mean_node_count       = (double)total_node_count / (double)POSITION_COUNT;
        double mean_nodes_per_second = total_nodes_per_second / (double)POSITION_COUNT;

        printf("\nResults for %s:\n", file_name);
        printf("Passed %zu/%zu tests\n", passed_tests, POSITION_COUNT);

        if (failed_test != (size_t)-1)
            printf("First failed test was %s: got %d, expected %d\n", test_positions[failed_test], failed_test_score,
                   failed_test_answer);

        printf("Total time:        %.6f s\n", (double)total_time / 1000000000.0);
        printf("Mean time:         %.6f s\n", mean_time / 1000000000.0);
        printf("Mean node count:   %zu\n", (size_t)mean_node_count);
        printf("Nodes per second:  %.2f kN/s\n", mean_nodes_per_second / 1000.0);
    }

    destroy_tt(tt);
}
