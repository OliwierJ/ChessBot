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

void DrawEndGameState(ChessGame &game, const Texture2D &piecesTexture, const Vector2 mouse) {
    for (auto &p: game.board().pieceList) {
        p.Draw(piecesTexture);
    }
    DrawRectangle(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, {100, 100, 100, 150});
    if (game.state().state == GameStatus::Checkmate) {
        const std::string text = game.state().winner == PieceColor::White ? "White wins!" : "Black wins!";
        DrawText("Checkmate!", 300, 300, 30, WHITE);
        DrawText(text.c_str(), 300, 330, 30, WHITE);
    } else {
        DrawText("Stalemate!", 300, 300, 30, WHITE);
    }

    DrawRectangle(260, 380, 225, 60, BLACK);
    DrawRectangle(265, 385, 215, 50, WHITE);
    DrawText("Restart", 325, 400, 28, BLACK);

    if (CheckCollisionPointRec(mouse, {260, 380, 225, 60}) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        game.restart_game(piecesTexture);
    }
}

void DrawButton(const Rectangle rect, const Color color, const char* text) {
    constexpr int font_size = 25;
    constexpr int border = 5;
    const auto [x, y] = MeasureTextEx(GetFontDefault(), text, font_size, 0);
    DrawRectangle(rect.x, rect.y, rect.width, rect.height, color);
    DrawRectangle(rect.x + border, rect.y + border, rect.width - border*2, rect.height - border*2, color);
    const Vector2 button_center = {rect.x + rect.width / 2, rect.y + rect.height / 2};
    const Vector2 text_point = {button_center.x - x / 2, button_center.y - y / 2};
    DrawText(text, text_point.x, text_point.y, font_size, BLACK);
}

void render_menu(ChessGame& game, const Vector2 mouse) {
    constexpr int button_w = 250;
    constexpr int button_y = 300;
    constexpr int bot_button_y = 400;
    constexpr int button_h = 50;
    constexpr Rectangle two_player_btn = {WINDOW_WIDTH / 2 - button_w / 2, button_y, button_w, button_h};
    constexpr Rectangle bot_game = {WINDOW_WIDTH / 2 - button_w / 2, bot_button_y, button_w, button_h};

    DrawButton(two_player_btn, WHITE, "Two player");
    DrawButton(bot_game, WHITE, "Bot game");

    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
        if (CheckCollisionPointRec(mouse, two_player_btn)) {
            game.state().bot_game = false;
            game.state().state = GameStatus::Normal;
        }
        if (CheckCollisionPointRec(mouse, bot_game)) {
            game.state().bot_game = true;
            game.state().state = GameStatus::Normal;
        }
    }
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

        // Get input from user
        const Vector2 mouse = GetMousePosition();

        if (game.state().state == GameStatus::Menu) {
            render_menu(game, mouse);
            EndDrawing();
            continue;
        }
        Board::Draw();
        game.board().draw_taken_material(piecesTexture);

        // Move history
        game.state().move_history.draw();

        // Game end loop
        if (game.state().state == GameStatus::Checkmate || game.state().state == GameStatus::Stalemate) {
            DrawEndGameState(game, piecesTexture, mouse);
            EndDrawing();
            continue;
        }


        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            std::cout << "X: " << mouse.x << "Y: " << mouse.y << "\n";
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
