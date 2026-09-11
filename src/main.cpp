#include <algorithm>
#include <fstream>
#include <future>
#include <iostream>
#include <limits>
#include "Board.h"
#include "Bot.h"
#include "ChessGame.h"
#include "GameState.h"
#include "raylib.h"
#include "Piece.h"
#include "Renderer.h"
#include "SoundManager.h"

void check_drop_position(Piece *&currentPiece, ChessGame &game) {
    if (currentPiece == nullptr) return;

    const std::string previousPosition = currentPiece->square->name;

    for (auto &[current_square, square]: game.board().squares) {
        if (!CheckCollisionPointRec(GetMousePosition(), square.squareBox)) {
            continue;
        }

        if (const auto move_result = game.try_move(*currentPiece, square)) {
            SoundManager::play_move_sound(game, move_result.value());
            currentPiece = nullptr;
        } else {
            currentPiece->reset_position();
        }
        return;
    }

    currentPiece->reset_position();
}

int main() {
    Renderer renderer;
    SoundManager::load_sounds();
    std::cout << std::boolalpha;

    ChessGame game(renderer.piecesTexture);
    Piece *currentPiece = nullptr;

    bool bot_thinking = false;
    std::future<std::optional<BotMove> > bot_task;

    while (!WindowShouldClose()) {
        const Vector2 mouse = GetMousePosition();

        BeginDrawing();
        renderer.draw(game, mouse, {false, currentPiece});
        EndDrawing();

        // Check if valid piece selection
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            if (currentPiece && currentPiece->isCurrentlyHeld) continue;
            for (auto &p: game.board().pieceList) {
                if (p.captured) continue;
                if (game.state().turn != p.colour) continue;
                if (game.state().bot_game && game.state().turn == PieceColor::Black) continue;
                if (CheckCollisionPointRec(mouse, p.boundingBox)) {
                    p.isCurrentlyHeld = true;
                    currentPiece = &p;
                    currentPiece->lastPosition = currentPiece->getCurrentPos();
                    break;
                }
            }
        }

        // Move current piece with mouse
        if (currentPiece && IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
            currentPiece->setCurrentPos({mouse.x - currentPiece->boundingBox.width / 2, mouse.y - currentPiece->boundingBox.height / 2});
        }

        // Check and dropped piece
        if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
            check_drop_position(currentPiece, game);
        }

        // Bot selection
        if (game.state().bot_game && game.state().turn == PieceColor::Black && !bot_thinking) {
            Board board_snapshot = game.board();
            GameState state_snapshot = game.state();

            bot_thinking = true;
            bot_task = std::async(
                std::launch::async,
                &Bot::choose_move,
                board_snapshot,
                state_snapshot,
                MAX_DEPTH,
                std::numeric_limits<float>::lowest(),
                std::numeric_limits<float>::max()
            );
        }

        if (bot_thinking && bot_task.wait_for(std::chrono::milliseconds(0)) == std::future_status::ready) {
            const auto move = bot_task.get();
            if (!move.has_value()) {
                bot_thinking = false;
                continue;
            }
            const auto [from, target] = move.value();
            Piece *piece = game.board().squares.at(from).piece;
            BoardSquare &square = game.board().squares.at(target);

            if (const auto result = game.try_move(*piece, square)) {
                SoundManager::play_move_sound(game, result.value());
            }

            bot_thinking = false;
        }
    }

    SoundManager::unload_sounds();
    return 0;
}
