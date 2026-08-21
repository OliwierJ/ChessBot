#include <algorithm>
#include <fstream>
#include <iostream>
#include <thread>
#include <vector>
#include "Board.h"
#include "GameState.h"
#include "MoveHistory.h"
#include "MoveValidator.h"
#include "raylib.h"
#include "Piece.h"
#include "embedded_resources.h"

auto TITLE = "Chess";

constexpr int WINDOW_WIDTH = 1150;
constexpr int WINDOW_HEIGHT = 750;

Sound checkSound;
Sound moveSound;
Sound illegalMoveSound;
Sound captureSound;
Sound promoteSound;
Sound gameEndSound;


void play_move_sound(const Board &board, const MoveOutcome move_outcome) {
    if (board.whiteIsChecked || board.blackIsChecked) {
        PlaySound(checkSound);
    } else if (move_outcome.pieceTaken) {
        PlaySound(captureSound);
    } else if (move_outcome.pawnPromoted) {
        PlaySound(promoteSound);
    } else {
        PlaySound(moveSound);
    }
}

bool finalize_move(Piece *&currentPiece, const Board &board, GameState &game, const std::string &previousPosition,
                   const BoardSquare &square, const MoveOutcome move_outcome) {
    const auto legalMoveCount = board.getLegalMoveCount(!game.turn);

    if (board.isColourChecked(!game.turn)) {
        if (legalMoveCount == 0) {
            game.state = CHECKMATE;
            game.winner = game.turn;
            PlaySound(gameEndSound);
        }
    }
    if (legalMoveCount == 0 && !board.isColourChecked(!game.turn)) {
        game.state = STALEMATE;
        PlaySound(gameEndSound);
    }

    // calculate move notation
    std::string move_notation = {static_cast<char>(square.name[0] + 32), square.name[1]};
    if (move_outcome.pieceTaken && currentPiece->type != PAWN) {
        move_notation = {currentPiece->getPieceNotation(), 'x', move_notation[0], move_notation[1]};
    } else if (move_outcome.pieceTaken) {
        move_notation = {static_cast<char>(previousPosition[0] + 32), 'x', move_notation[0], move_notation[1]};
    } else if (move_outcome.shortCastled) {
        move_notation = "O-O";
    } else if (move_outcome.longCastled) {
        move_notation = "O-O-O";
    } else {
        move_notation = {currentPiece->getPieceNotation(), move_notation[0], move_notation[1]};
    }
    if (board.isColourChecked(!game.turn) && game.state != CHECKMATE) move_notation += '+';
    if (game.state == CHECKMATE) move_notation += '#';

    game.move_history.append_move(move_notation);

    if (game.state == CHECKMATE || game.state == STALEMATE) return true;
    game.turn = !game.turn;
    return false;
}

void check_drop_position(Piece *&currentPiece, Board &board, GameState &game) {
    if (currentPiece == nullptr) return;

    bool foundValidMove = false;
    const std::string previousPosition = currentPiece->square->name;

    for (auto &[current_square, square]: board.squares) {
        if (CheckCollisionPointRec(GetMousePosition(), square.squareBox)) {
            if (!MoveValidator::validate_legal_move(currentPiece, square, board)) break;

            MoveOutcome move_outcome;
            MoveValidator::apply_move(currentPiece, board, square, move_outcome, game.turn);
            if (finalize_move(currentPiece, board, game, previousPosition, square, move_outcome)) return;
            foundValidMove = true;
            play_move_sound(board, move_outcome);
            break;
        }
    }

    if (!foundValidMove) {
        currentPiece->setCurrentPos({currentPiece->lastPosition.x, currentPiece->lastPosition.y});
        board.calculateAllLegalMovesByColour(game.turn);
    } else {
        currentPiece = nullptr;
    }
}

void DrawEndGameState(const GameState &game, const Board &board, const Texture2D &piecesTexture) {
    for (auto &p: board.pieceList) {
        p.Draw(piecesTexture);
    }
    DrawRectangle(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, {100, 100, 100, 150});
    if (game.state == CHECKMATE) {
        const std::string text = game.winner == 0 ? "White wins!" : "Black wins!";
        DrawText("Checkmate!", 300, 300, 30, WHITE);
        DrawText(text.c_str(), 300, 330, 30, WHITE);
    } else {
        DrawText("Stalemate!", 300, 300, 30, WHITE);
    }

    DrawRectangle(260, 380, 225, 60, BLACK);
    DrawRectangle(265, 385, 215, 50, WHITE);
    DrawText("Restart", 325, 400, 28, BLACK);

}

void load_sounds() {
    auto load_sound = [](const char *fileType, const unsigned char *data, const std::size_t size) {
        const Wave wave = LoadWaveFromMemory(fileType, data, static_cast<int>(size));
        const Sound sound = LoadSoundFromWave(wave);
        UnloadWave(wave);
        return sound;
    };

    checkSound = load_sound(".wav", move_check_wav, move_check_wav_size);
    moveSound = load_sound(".wav", move_self_wav, move_self_wav_size);
    illegalMoveSound = load_sound(".wav", illegal_wav, illegal_wav_size);
    captureSound = load_sound(".mp3", capture_mp3, capture_mp3_size);
    promoteSound = load_sound(".mp3", promote_mp3, promote_mp3_size);
    gameEndSound = load_sound(".mp3", game_end_mp3, game_end_mp3_size);
}

void perform_bot_move(GameState &game, Board &board) {
    if (game.bot_game && game.turn) {
        bool found_piece = false;
        Piece *random_piece = nullptr;
        std::string move;
        while (!found_piece) {
            const int randomPieceIndex = std::rand() % board.pieceList.size();
            random_piece = &board.pieceList[randomPieceIndex];
            if (random_piece->captured) continue;
            if (random_piece->colour != game.turn) continue;
            if (random_piece->legalMoves.empty()) continue;

            const int randomMoveIndex = std::rand() % random_piece->legalMoves.size();
            move = random_piece->legalMoves[randomMoveIndex];
            if (!MoveValidator::validate_legal_move(random_piece, board.squares[move], board)) continue;
            found_piece = true;
        }
        // std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        const std::string previousPosition = random_piece->square->name;
        MoveOutcome move_outcome;
        MoveValidator::apply_move(random_piece, board, board.squares[move], move_outcome, game.turn);
        finalize_move(random_piece, board, game, previousPosition, board.squares[move], move_outcome);
        play_move_sound(board, move_outcome);
    }
}

int main() {
    // initialise
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, TITLE);
    InitAudioDevice();
    load_sounds();
    std::cout << std::boolalpha;
    const Image piecesImage = LoadImageFromMemory(".png", Chess_Pieces_Sprite_png, Chess_Pieces_Sprite_png_size);
    const Texture2D piecesTexture = LoadTextureFromImage(piecesImage);
    UnloadImage(piecesImage);
    srand(time(nullptr));

    GameState game;
    const MoveHistory move_history;
    game.move_history = move_history;

    Board board;
    board.set_up_pieces(piecesTexture);
    Piece *currentPiece = nullptr;

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(DARKBROWN);
        Board::Draw();

        // Move history
        game.move_history.draw();

        // Game end loop
        if (game.state == CHECKMATE || game.state == STALEMATE) {
            DrawEndGameState(game, board, piecesTexture);
            EndDrawing();
            continue;
        }

        // Get input from user
        const Vector2 mouse = GetMousePosition();

        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            for (auto &p: board.pieceList) {
                if (p.captured) continue;
                if (game.turn != p.colour) continue;
                if (game.bot_game && game.turn) continue;
                if (CheckCollisionPointRec(mouse, p.boundingBox)) {
                    p.isCurrentlyHeld = true;
                    currentPiece = &p;
                    currentPiece->lastPosition = currentPiece->getCurrentPos();
                    break;
                }
            }
        }
        // Move held piece with cursor
        for (auto &p: board.pieceList) {
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
            check_drop_position(currentPiece, board, game);
            Board::Draw();
            game.move_history.draw();
            for (auto &p: board.pieceList) {
                p.Draw(piecesTexture);
            }
        }

        // Draw held piece and its legal moves
        if (currentPiece != nullptr) {
            for (std::string &legalMove: currentPiece->legalMoves) {
                board.drawLegalMove(legalMove, currentPiece->colour);
            }
            currentPiece->Draw(piecesTexture);
        }


        if (!game.turn)
            DrawText("White's turn", 10, 10, 20, WHITE);
        if (game.turn)
            DrawText("Black's turn", 10, 10, 20, WHITE);

        EndDrawing();
        perform_bot_move(game, board);
    }

    UnloadTexture(piecesTexture);
    UnloadSound(checkSound);
    UnloadSound(moveSound);
    UnloadSound(illegalMoveSound);
    UnloadSound(captureSound);
    UnloadSound(promoteSound);
    UnloadSound(gameEndSound);
    CloseAudioDevice();
    CloseWindow();
    return 0;
}
