# ChessBot

ChessBot is a C++23 chess prototype built with [raylib](https://www.raylib.com/). The codebase focuses on an interactive board, robust move generation, and a compact game-state system for local play.

## Current Progress

The project has progressed from a visual board to a lightweight rules-capable game. Key implemented features:

- Full 8x8 board rendering and coordinate mapping.
- Piece rendering from a sprite sheet and drag-and-drop interaction.
- Legal move generation for pawns, knights, bishops, rooks, queens and kings.
- Capture handling and taken-state tracking.
- Turn-based play with white/black switching.
- Check detection: moves that leave a king in check are rejected.
- En passant support: capture squares are tracked and applied when valid.
- Castling: king-side and queen-side moves are handled (with rook relocation).
- Pawn promotion: basic promotion to queen.
- Endgame detection: checkmate and stalemate are detected and set the `GameState`.
- Audio feedback: sounds for move, capture, check, illegal move, promotion, and game end.

## Project Structure

- `main.cpp`: game loop, input handling, `checkDropPosition`, game-state transitions, audio lifecycle.
- `Board.h` / `Board.cpp`: board model, square lookup, legal-move orchestration, and helpers.
- `Piece.h` / `Piece.cpp`: piece state, move generation, promotion, and king-attacked helpers.
- `BoardSquare.h` / `BoardSquare.cpp`: per-square container and name/rectangle mapping.
- `GameState.h` / `GameState.cpp`: simple game-state enum and winner tracking.
- `CMakeLists.txt`: build configuration (C++23, fetches raylib if missing).

## What Is Working

1. Pieces render and can be picked up, dragged, and dropped.
2. Move generation enforces standard movement rules and displays legal targets.
3. Illegal moves that would leave a king in check are rejected.
4. En passant, castling, and basic pawn promotion are supported.
5. Checkmate and stalemate are detected via legal-move counts and check state.
6. Audio events play for moves, captures, checks, promotions, illegal attempts, and game end.
7. End-game overlay displays winner or stalemate messages.

## Known Gaps & Notes

- Promotion currently auto-promotes to queen via `try_promote()`; a promotion UI is not implemented.
- The rules engine is functional but not exhaustively tested; edge cases could remain.
- Piece ownership uses a `std::vector<Piece>` and stores raw `Piece*` in squares — be cautious if changing container semantics (promotion or removal could invalidate pointers without reserve).

## Build

The project uses CMake and targets C++23. If `raylib` is not installed locally, CMake will fetch it.

Typical build flow:

```bash
cmake -S . -B build
cmake --build build
```

## Run

After building, run the executable from the build folder. The program opens a 1150x750 window and includes an audio device. Audio and image resources are embedded into the executable during the CMake configure step, so the `resources/` directory is not needed at runtime.

## Next Steps

Recommended next work (high priority):

1. Add a promotion selection UI to allow choosing the promoted piece.
2. Harden simulation/rollback logic in `checkDropPosition` so temporary state (captured flags, en-passant removal) is always restored on rejected moves.
3. Add unit or harness tests for move-generation edge cases (en passant, discovered checks, promotions).
4. Replace header-level `static` globals with constants or scoped objects to avoid duplication across translation units.
5. Consider stable storage for `Piece` objects (e.g., `std::deque` or owning pointers) to eliminate pointer-invalidations when expanding the collection.

