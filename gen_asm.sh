#!/usr/bin/env bash

objdump -d -x -Mintel --demangle --no-show-raw-insn "src/foursight" > "src/foursight.S"
