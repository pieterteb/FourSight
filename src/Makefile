# Compiler and flags
CC      := clang
BASE    := -std=c11 -fdiagnostics-color=always -Werror -Wpedantic -Wall -Wextra -Wdouble-promotion -Wformat=2 -Wformat-signedness -Wnull-dereference -Winit-self -Wmissing-include-dirs -Wunused -Wuninitialized -Wstrict-overflow=2 -Wzero-as-null-pointer-constant -Wshadow -Wundef -Wbad-function-cast -Wcast-qual -Wcast-align -Wwrite-strings -Wconversion -Wstrict-prototypes -Wpacked -Wredundant-decls -Wnested-externs -Winline -Winvalid-utf8 -D_GNU_SOURCE
DEBUG   := -g -O0 -fsanitize=address -fstack-protector-all
RELEASE := -O3 -march=native -flto -fno-stack-protector -DNDEBUG

# Sources, objects, target
SRC     := main.c benchmark.c bitboard.c move_ordering.c position.c search.c transposition_table.c
OBJ     := $(SRC:.c=.o)
TARGET  := foursight

# Default build: release
CFLAGS  := $(BASE) $(RELEASE)

# Targets
.PHONY: all debug release clean run

all: release

debug: CFLAGS := $(BASE) $(DEBUG)
debug: clean $(TARGET)

release: CFLAGS := $(BASE) $(RELEASE)
release: clean $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f $(OBJ) $(TARGET)

run: $(TARGET)
	./$(TARGET)
