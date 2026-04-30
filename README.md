# ChessBot

ChessBot is a C++23 chess board prototype built with [raylib](https://www.raylib.com/). The current codebase focuses on rendering a playable board, loading and drawing the piece set, and supporting drag-and-drop movement with basic move validation.

## Current Progress

The project is already past the initial rendering stage and now has a working interactive board. The main progress so far is:

- A full 8x8 board is generated and drawn with square coordinates.
- All standard chess pieces are created and placed on the board, with setup still being refined in a few spots.
- Pieces can be selected with the mouse, dragged across the board, and dropped onto squares.
- Basic legal-move checking exists for pawns, knights, bishops, rooks, queens, and kings.
- Captures are supported by marking opposing pieces as taken.
- Legal move hints are displayed for the currently selected piece.
- Turn switching is implemented between white and black.

## Project Structure

- [main.cpp](main.cpp) contains the game loop, input handling, turn logic, and board interaction.
- [Board.cpp](Board.cpp) and [Board.h](Board.h) define the board grid, square generation, coordinate helpers, and legal move markers.
- [Piece.cpp](Piece.cpp) and [Piece.h](Piece.h) implement piece state, drawing, and move generation.
- [BoardSquare.cpp](BoardSquare.cpp) and [BoardSquare.h](BoardSquare.h) define the per-square data model.
- [CMakeLists.txt](CMakeLists.txt) configures the C++23 build and pulls in raylib if it is not installed locally.

## What Is Working

The current prototype can run as a visual chess board and supports the following behavior:

1. Pieces are rendered from a sprite sheet.
2. Moves are validated against the piece-specific move rules currently implemented.
3. The board rejects obvious invalid drops, such as moving to a square outside the generated move set.
4. Friendly pieces cannot be captured.
5. Legal squares are highlighted while a piece is held.

## Known Gaps

The project is still a prototype, so several chess rules and polish items are not implemented yet:

- No check, checkmate, stalemate, or draw detection.
- No castling.
- No en passant.
- No pawn promotion.
- No move history, undo, or notation export.
- No AI or engine opponent yet.
- The current focus is on move generation and board interaction rather than full rules enforcement.

## Build

The project uses CMake and targets C++23.

If raylib is available locally, CMake will use it. Otherwise, the build pulls raylib with FetchContent.

Typical build flow:

```bash
cmake -S . -B build
cmake --build build
```

## Run

After building, run the generated executable from the build directory. The program opens a 750x750 window and renders the chess board immediately.

## Next Steps

The most important follow-up work is to turn the current move generator into a complete chess rules engine. Good next milestones are check detection, castling, promotion, and a simple game state system for tracking wins and draws.
