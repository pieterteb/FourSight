# FourSight

FourSight is a strong Connect 4 solver for Linux. It computes the best move for any legal position.

Assuming perfect play from the opponent:
- If a position is winnable, FourSight finds the fastest path to victory.
- If a position is not winnable, it finds the longest path to a draw or loss.

The goal of FourSight is not to solve the entire game at once, but to solve individual positions as quickly as possible.
Every position is solved from scratch: no precomputed solutions or opening books are used.

Currently, FourSight can solve the start position in a little under 4 minutes.

## Installation

```bash
git clone https://github.com/pieterteb/FourSight.git
cd FourSight/src
make
```

## Options

FourSight supports three main modes:

### Help

Displays usage instructions:

```bash
./foursight help
```

### Benchmark

Runs performance tests using predefined positions:

```bash
./foursight bench <file_count>
```

- `<file_count>`: Number of test files to process (from the `tests` directory).
- Test positions sourced from:  
  http://blog.gamesolver.org/solving-connect-four/02-test-protocol/.


### Solve a Position

```bash
./foursight <position>
```

- `<position>`: A string of column numbers representing a sequence of moves.
- If omitted, FourSight assumes an empty board.

---

## Example

```bash
./src/foursight 2531276566711153
```

Output:

```bash
+---+---+---+---+---+---+---+
|   |   |   |   |   |   |   |
+---+---+---+---+---+---+---+
|   |   |   |   |   |   |   |
+---+---+---+---+---+---+---+
| Y |   |   |   |   |   |   |
+---+---+---+---+---+---+---+
| R |   |   |   | R | Y |   |
+---+---+---+---+---+---+---+
| Y | R | Y |   | Y | R | R |
+---+---+---+---+---+---+---+
| Y | R | R |   | Y | R | Y |
+---+---+---+---+---+---+---+
  1   2   3   4   5   6   7
To move:    Red

Best move:         7
Score:             2
Time:              0.022132 s
Nodes searched:    232905
Nodes per second:  10523.23 kN/s
```

The score is positive if it is a win, negative if it is a loss, and 0 if it is a draw.
The magnitude indicates how many tokens are left when the last move is played: 1 if the game is won/lost with the last token, 2 if the game is won/lost with the second to last token etc.
