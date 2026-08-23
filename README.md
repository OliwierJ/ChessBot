# ChessBot

ChessBot is a local C++23 chess game built with [raylib](https://www.raylib.com/). It provides a graphical board, legal move validation, move history, sound effects, and a simple random-move opponent.

## Features

- Drag-and-drop play on a rendered 8x8 board.
- Legal move generation for all standard pieces.
- Turn enforcement and rejection of moves that leave the moving side's king in check.
- Captures, en passant, king-side castling, and queen-side castling.
- Automatic pawn promotion to a queen.
- Check, checkmate, and stalemate detection.
- Move history displayed in the game window using compact algebraic-style notation.
- Audio feedback for moves, captures, checks, illegal moves, promotion, and game end.
- Human White versus a random-move Black bot by default.

## Requirements

- CMake 4.0 or newer.
- A C++ compiler with C++23 support.
- Git, for CMake's `FetchContent` fallback.
- An audio-capable desktop environment.

raylib 5.5 and Catch2 3.8.1 are fetched automatically when they are not already available to CMake.

## Build

From the repository root:

```bash
cmake -S . -B build
cmake --build build
```

The build produces two executables:

- `ChessBot`: the graphical game.
- `ChessBotTests`: the Catch2 test suite.

The configure step embeds the sprite sheet and audio files from `resources/` into generated C++ data. The source `resources/` directory is therefore not required when running an already-built executable.

## Run

Run `ChessBot` from the build directory. The game opens a 1150x750 window. Pick up a piece with the left mouse button, move it over a highlighted legal square, and release it. White is controlled by the player; Black makes a randomly selected legal move after White's turn.

## Test

Run the tests with CTest:

```bash
ctest --test-dir build --output-on-failure
```

The tests cover board setup, piece movement, captures, check restrictions, and basic game-turn behavior. The rules engine is not exhaustively tested, so unusual positions may still expose edge cases.

## Project Structure

- `src/main.cpp`: raylib window, input loop, rendering, bot turn, and audio lifecycle.
- `src/ChessGame.*`: turn handling, move orchestration, notation, and end-game status.
- `src/Board.*`: board model, square lookup, legal-move calculation, and board rendering.
- `src/Piece.*`: piece state, movement generation, promotion, and attack helpers.
- `src/MoveValidator.*`: legal-move checks and move application.
- `src/BoardSquare.*`: square coordinates, rectangles, and occupied-piece state.
- `src/MoveHistory.*`: move storage and on-screen history rendering.
- `src/GameState.*`: turn, game status, winner, and bot-mode state.
- `src/SoundManager.*`: embedded sound loading and playback.
- `tests/`: Catch2 unit tests.
- `resources/`: the sprite sheet and sound effects embedded at configure time.

## Known Limitations

- Promotion always selects a queen; there is no promotion-choice UI.
- The bot chooses moves randomly and has no search or position evaluation.
- The game is configured for bot mode by default; there is no start screen or two-player mode selector.
- The move history displays a moving window and cannot be scrolled manually.
- The end-game overlay displays a `Restart` control, but restarting is not wired up yet.

## Roadmap

1. Add game reset and a start screen.
2. Add a two-player mode selector.
3. Replace random bot moves with search and position evaluation.
