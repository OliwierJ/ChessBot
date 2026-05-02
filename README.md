# ChessBot

ChessBot is a C++23 chess board prototype built with [raylib](https://www.raylib.com/). The current codebase focuses on rendering a playable board, loading and drawing the piece set, and supporting drag-and-drop movement with basic move validation.

## Current Progress

The project now has a fully functional interactive chess board with check detection and audio feedback. Recent updates include:

- A full 8x8 board is generated and drawn with square coordinates.
- All standard chess pieces are created and placed on the board, with setup still being refined in a few spots.
- Pieces can be selected with the mouse, dragged across the board, and dropped onto squares.
- Basic legal-move checking exists for pawns, knights, bishops, rooks, queens, and kings.
- Captures are supported by marking opposing pieces as taken.
- Legal move hints are displayed for the currently selected piece.
- Turn switching is implemented between white and black.
- **Check detection is now implemented** — moves that leave a king in check are rejected and cannot be played.
- **Sound effects** are triggered for moves, captures, check, and illegal moves.
- **Refactored board management** — the Board class now manages piece placement and calculates all legal moves centrally.

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
3. Moves that leave a player's king in check are rejected and trigger an illegal-move sound.
4. Friendly pieces cannot be captured.
5. Legal squares are highlighted while a piece is held.
6. Audio feedback plays for moves, captures, checks, and illegal moves.

## Known Gaps

The project is still a prototype, so several chess rules and polish items are not implemented yet:

- No checkmate, stalemate, or draw detection yet.
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

With check detection in place, the next priorities are:
1. **Checkmate and stalemate detection** to properly end games.
2. **Castling and en passant** to complete the core rule set.
3. **Pawn promotion** to allow pawns to convert at the end ranks.
4. **Move history and game-state UI** for better user feedback and optional undo.
5. **Basic AI opponent** once the rules engine is stable.
