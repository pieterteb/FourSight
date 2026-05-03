// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Pieter te Brake

#ifndef FOURSIGHT_BENCHMARK_H_
#define FOURSIGHT_BENCHMARK_H_


#include <stddef.h>

#include "position.h"



static const size_t TEST_FILE_COUNT = 6;


struct Position parse_position(const char* position_string);

void benchmark(const unsigned file_count);



#endif  // #ifndef FOURSIGHT_BENCHMARK_H_
