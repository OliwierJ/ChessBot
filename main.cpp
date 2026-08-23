#include <algorithm>
#include <fstream>
#include <iostream>
#include <thread>
#include <vector>
#include "Board.h"
#include "ChessGame.h"
#include "GameState.h"
#include "MoveHistory.h"
#include "raylib.h"
#include "Piece.h"
#include "embedded_resources.h"
#include "SoundManager.h"

auto TITLE = "Chess";

constexpr int WINDOW_WIDTH = 1150;
constexpr int WINDOW_HEIGHT = 750;

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


void DrawEndGameState(const GameState &game, const Board &board, const Texture2D &piecesTexture) {
    for (auto &p: board.pieceList) {
        p.Draw(piecesTexture);
    }
    DrawRectangle(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, {100, 100, 100, 150});
    if (game.state == GameStatus::Checkmate) {
        const std::string text = game.winner == PieceColor::White ? "White wins!" : "Black wins!";
        DrawText("Checkmate!", 300, 300, 30, WHITE);
        DrawText(text.c_str(), 300, 330, 30, WHITE);
    } else {
        DrawText("Stalemate!", 300, 300, 30, WHITE);
    }

    DrawRectangle(260, 380, 225, 60, BLACK);
    DrawRectangle(265, 385, 215, 50, WHITE);
    DrawText("Restart", 325, 400, 28, BLACK);
}

int main() {
    // initialise
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, TITLE);
    InitAudioDevice();
    SoundManager::load_sounds();
    std::cout << std::boolalpha;
    const Image piecesImage = LoadImageFromMemory(".png", Chess_Pieces_Sprite_png, Chess_Pieces_Sprite_png_size);
    const Texture2D piecesTexture = LoadTextureFromImage(piecesImage);
    UnloadImage(piecesImage);
    srand(time(nullptr));

    ChessGame game(piecesTexture);
    Piece *currentPiece = nullptr;

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(DARKBROWN);
        Board::Draw();

        // Move history
        game.state().move_history.draw();

        // Game end loop
        if (game.state().state == GameStatus::Checkmate || game.state().state == GameStatus::Stalemate) {
            DrawEndGameState(game.state(), game.board(), piecesTexture);
            EndDrawing();
            continue;
        }

        // Get input from user
        const Vector2 mouse = GetMousePosition();

        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
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
        // Move held piece with cursor
        for (auto &p: game.board().pieceList) {
            if (p.isCurrentlyHeld) {
                if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
                    p.setCurrentPos({mouse.x - p.boundingBox.width / 2, mouse.y - p.boundingBox.height / 2});
                } else {
                    p.isCurrentlyHeld = false;
                }
            }
            if (currentPiece != nullptr && p.id == currentPiece->id) continue;
            p.Draw(piecesTexture);
        }

        if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
            check_drop_position(currentPiece, game);
            Board::Draw();
            game.state().move_history.draw();
            for (auto &p: game.board().pieceList) {
                p.Draw(piecesTexture);
            }
        }

        // Draw held piece and its legal moves
        if (currentPiece != nullptr) {
            for (std::string &legalMove: currentPiece->legalMoves) {
                game.board().drawLegalMove(legalMove, currentPiece->colour);
            }
            currentPiece->Draw(piecesTexture);
        }

        if (game.state().turn == PieceColor::White)
            DrawText("White's turn", 10, 10, 20, WHITE);
        else
            DrawText("Black's turn", 10, 10, 20, WHITE);

        EndDrawing();
        if (game.state().bot_game && game.state().turn == PieceColor::Black) {
            if (const auto result = game.perform_bot_move()) {
                SoundManager::play_move_sound(game, result.value());
            }
        }
    }

    UnloadTexture(piecesTexture);
    SoundManager::unload_sounds();
    CloseAudioDevice();
    CloseWindow();
    return 0;
}
