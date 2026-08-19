#include <algorithm>
#include <fstream>
#include <iostream>
#include <vector>
#include "Board.h"
#include "GameState.h"
#include "MoveHistory.h"
#include "MoveValidator.h"
#include "raylib.h"
#include "Piece.h"

auto TITLE = "Chess";

static bool turn = PIECE_WHITE;
constexpr int WINDOW_WIDTH = 1150;
constexpr int WINDOW_HEIGHT = 750;

Sound checkSound;
Sound moveSound;
Sound illegalMoveSound;
Sound captureSound;
Sound promoteSound;
Sound gameEndSound;

// Loop each square and find the target square
BoardSquare find_target_square(Board &board) {
    for (auto &[current_square, square]: board.squares) {
        // Found target square
        if (CheckCollisionPointRec(GetMousePosition(), square.squareBox)) {
            return square;
        }
    }
    return {};
}

void check_drop_position(Piece *&currentPiece, Board &board, GameState &state) {
    if (currentPiece == nullptr) return;
    bool foundValidMove = false;
    bool pieceTaken = false;
    bool pawnPromoted = false;
    bool shortCastled = false;
    bool longCastled = false;
    const std::string previousPosition = currentPiece->square->name;

    BoardSquare target_square = find_target_square(board);

    if (!target_square.name.empty()) {
         foundValidMove = MoveValidator::validate_legal_move(currentPiece, target_square, board, state);

        if (std::ranges::count(board.enpassantSquares, target_square.name)) {
            const int upOrDownMove = currentPiece->colour == PIECE_WHITE ? -1 : 1;
            const std::string takenSquare = {
                (target_square.name[0]), static_cast<char>(target_square.name[1] + upOrDownMove)
            };

            board.squares[takenSquare].piece->taken = true;
            board.squares[takenSquare].piece = nullptr;
            pieceTaken = true;
        }
        board.enpassantSquares.clear();

        if (!currentPiece->hasMoved) {
            currentPiece->hasMoved = true;
            if (currentPiece->type == "king") {
                if (currentPiece->colour == PIECE_WHITE) {
                    if (target_square.name == "G1") {
                        const auto rRook = board.squares["H1"].piece;
                        rRook->setCurrentPos({board.squares["F1"].squareBox.x, board.squares["F1"].squareBox.y});
                        rRook->lastPosition = currentPiece->getCurrentPos();
                        rRook->square->piece = nullptr;
                        board.squares["F1"].piece = rRook;
                        rRook->square = &board.squares["F1"];
                        shortCastled = true;
                    }
                    if (target_square.name == "C1") {
                        const auto lRook = board.squares["A1"].piece;
                        lRook->setCurrentPos({board.squares["D1"].squareBox.x, board.squares["D1"].squareBox.y});
                        lRook->lastPosition = currentPiece->getCurrentPos();
                        lRook->square->piece = nullptr;
                        board.squares["D1"].piece = lRook;
                        lRook->square = &board.squares["D1"];
                        longCastled = true;
                    }
                } else {
                    if (target_square.name == "G8") {
                        const auto rRook = board.squares["H8"].piece;
                        rRook->setCurrentPos({board.squares["F8"].squareBox.x, board.squares["F8"].squareBox.y});
                        rRook->lastPosition = currentPiece->getCurrentPos();
                        rRook->square->piece = nullptr;
                        board.squares["F8"].piece = rRook;
                        rRook->square = &board.squares["F8"];
                        shortCastled = true;
                    }
                    if (target_square.name == "C8") {
                        const auto lRook = board.squares["A8"].piece;
                        lRook->setCurrentPos({board.squares["D8"].squareBox.x, board.squares["D8"].squareBox.y});
                        lRook->lastPosition = currentPiece->getCurrentPos();
                        lRook->square->piece = nullptr;
                        board.squares["D8"].piece = lRook;
                        lRook->square = &board.squares["D8"];
                        longCastled = true;
                    }
                }
            }

            if (currentPiece->type == "rook") {
                if (currentPiece->colour == PIECE_WHITE) {
                    if (currentPiece->square->name == "A1") {
                        board.whiteCanLongCastle = false;
                    }
                    if (currentPiece->square->name == "H1") {
                        board.whiteCanShortCastle = false;
                    }
                } else {
                    if (currentPiece->square->name == "A8") {
                        board.blackCanLongCastle = false;
                    }
                    if (currentPiece->square->name == "H8") {
                        board.blackCanShortCastle = false;
                    }
                }
            }

            if (currentPiece->type == "pawn") {
                if (currentPiece->colour == PIECE_WHITE) {
                    if (target_square.name[1] == '4') {
                        board.enpassantSquares.push_back({target_square.name[0], '3'});
                    }
                } else {
                    if (target_square.name[1] == '5') {
                        board.enpassantSquares.push_back({target_square.name[0], '6'});
                    }
                }
            }
        }
        currentPiece->setCurrentPos({target_square.squareBox.x, target_square.squareBox.y});
        currentPiece->lastPosition = currentPiece->getCurrentPos();
        currentPiece->square->piece = nullptr;
        target_square.piece = currentPiece;
        currentPiece->square = &target_square;
        pawnPromoted = currentPiece->try_promote();
        board.calculateAllLegalMovesByColour(!turn);
        if (currentPiece->colour == PIECE_WHITE) {
            auto temp = board.attackedSquaresOfColor(PIECE_BLACK);
            board.blackIsChecked = std::ranges::count(temp, board.blackKing->square->name) >= 1;
        } else {
            auto temp = board.attackedSquaresOfColor(PIECE_WHITE);
            board.whiteIsChecked = std::ranges::count(temp, board.whiteKing->square->name) >= 1;
        }

        const auto legalMoveCount = board.getLegalMoveCount(!turn);
        if (board.isColourChecked(!turn)) {
            if (legalMoveCount == 0) {
                state.state = CHECKMATE;
                state.winner = turn;
                currentPiece = nullptr;
                PlaySound(gameEndSound);
                return;
            }
        }
        if (legalMoveCount == 0 && !board.isColourChecked(!turn)) {
            state.state = STALEMATE;
            currentPiece = nullptr;
            PlaySound(gameEndSound);
            return;
        }

        if (board.whiteIsChecked || board.blackIsChecked) {
            PlaySound(checkSound);
        } else if (pieceTaken) {
            PlaySound(captureSound);
        } else if (pawnPromoted) {
            PlaySound(promoteSound);
        } else {
            PlaySound(moveSound);
        }

        // calculate move notation
        std::string move_notation = {static_cast<char>(target_square.name[0] + 32), target_square.name[1]};
        if (pieceTaken && currentPiece->type != "pawn") {
            move_notation = {currentPiece->getPieceNotation(), 'x', move_notation[0], move_notation[1]};
        } else if (pieceTaken) {
            move_notation = {static_cast<char>(previousPosition[0] + 32), 'x', move_notation[0], move_notation[1]};
        } else if (shortCastled) {
            move_notation = "O-O";
        } else if (longCastled) {
            move_notation = "O-O-O";
        } else {
            move_notation = {currentPiece->getPieceNotation(), move_notation[0], move_notation[1]};
        }
        if (board.isColourChecked(!turn)) move_notation += '+';
        state.move_history.append_move(move_notation);
        state.move_history.print_history();

        foundValidMove = true;
        turn = !turn;
    }

    if (!foundValidMove) {
        currentPiece->setCurrentPos({currentPiece->lastPosition.x, currentPiece->lastPosition.y});
        board.calculateAllLegalMovesByColour(turn);
    } else {
        currentPiece = nullptr;
    }
}

void DrawEndGameState(const GameState &game, const Board &board, const Texture2D &piecesTexture) {
    for (auto &p: board.pieceList) {
        p.Draw(piecesTexture);
    }
    DrawRectangle(0, 0, 750, 750, {100, 100, 100, 150});
    if (game.state == CHECKMATE) {
        const std::string text = game.winner == 0 ? "White wins!" : "Black wins!";
        DrawText("Checkmate!", 300, 300, 30, WHITE);
        DrawText(text.c_str(), 300, 330, 30, WHITE);
    } else {
        DrawText("Stalemate!", 300, 300, 30, WHITE);
    }
}

void load_sounds() {
    checkSound = LoadSound("../resources/move-check.wav");
    moveSound = LoadSound("../resources/move-self.wav");
    illegalMoveSound = LoadSound("../resources/illegal.wav");
    captureSound = LoadSound("../resources/capture.mp3");
    promoteSound = LoadSound("../resources/promote.mp3");
    gameEndSound = LoadSound("../resources/game-end.mp3");
}

int main() {
    // initialise
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, TITLE);
    InitAudioDevice();
    load_sounds();
    std::cout << std::boolalpha;
    const Texture2D piecesTexture = LoadTexture("../resources/Chess_Pieces_Sprite.png");

    GameState game;
    const MoveHistory move_history;
    game.move_history = move_history;
    Board board;
    std::vector<Piece> *pieceList = &board.pieceList;

    pieceList->reserve(32);
    {
        // White Pieces
        board.addPieceToBoard("pawn", PAWN, PIECE_WHITE, "A2", piecesTexture);
        board.addPieceToBoard("pawn", PAWN, PIECE_WHITE, "B2", piecesTexture);
        board.addPieceToBoard("pawn", PAWN, PIECE_WHITE, "C2", piecesTexture);
        board.addPieceToBoard("pawn", PAWN, PIECE_WHITE, "D2", piecesTexture);
        board.addPieceToBoard("pawn", PAWN, PIECE_WHITE, "E2", piecesTexture);
        board.addPieceToBoard("pawn", PAWN, PIECE_WHITE, "F2", piecesTexture);
        board.addPieceToBoard("pawn", PAWN, PIECE_WHITE, "G2", piecesTexture);
        board.addPieceToBoard("pawn", PAWN, PIECE_WHITE, "H2", piecesTexture);
        board.addPieceToBoard("rook", ROOK, PIECE_WHITE, "A1", piecesTexture);
        board.addPieceToBoard("knight", KNIGHT, PIECE_WHITE, "B1", piecesTexture);
        board.addPieceToBoard("bishop", BISHOP, PIECE_WHITE, "C1", piecesTexture);
        board.addPieceToBoard("queen", QUEEN, PIECE_WHITE, "D1", piecesTexture);
        board.addPieceToBoard("king", KING, PIECE_WHITE, "E1", piecesTexture);
        board.whiteKing = &pieceList->back();
        board.addPieceToBoard("bishop", BISHOP, PIECE_WHITE, "F1", piecesTexture);
        board.addPieceToBoard("knight", KNIGHT, PIECE_WHITE, "G1", piecesTexture);
        board.addPieceToBoard("rook", ROOK, PIECE_WHITE, "H1", piecesTexture);
        // Black Pieces
        board.addPieceToBoard("pawn", PAWN, PIECE_BLACK, "A7", piecesTexture);
        board.addPieceToBoard("pawn", PAWN, PIECE_BLACK, "B7", piecesTexture);
        board.addPieceToBoard("pawn", PAWN, PIECE_BLACK, "C7", piecesTexture);
        board.addPieceToBoard("pawn", PAWN, PIECE_BLACK, "D7", piecesTexture);
        board.addPieceToBoard("pawn", PAWN, PIECE_BLACK, "E7", piecesTexture);
        board.addPieceToBoard("pawn", PAWN, PIECE_BLACK, "F7", piecesTexture);
        board.addPieceToBoard("pawn", PAWN, PIECE_BLACK, "G7", piecesTexture);
        board.addPieceToBoard("pawn", PAWN, PIECE_BLACK, "H7", piecesTexture);
        board.addPieceToBoard("rook", ROOK, PIECE_BLACK, "A8", piecesTexture);
        board.addPieceToBoard("knight", KNIGHT, PIECE_BLACK, "B8", piecesTexture);
        board.addPieceToBoard("bishop", BISHOP, PIECE_BLACK, "C8", piecesTexture);
        board.addPieceToBoard("queen", QUEEN, PIECE_BLACK, "D8", piecesTexture);
        board.addPieceToBoard("king", KING, PIECE_BLACK, "E8", piecesTexture);
        board.blackKing = &pieceList->back();
        board.addPieceToBoard("bishop", BISHOP, PIECE_BLACK, "F8", piecesTexture);
        board.addPieceToBoard("knight", KNIGHT, PIECE_BLACK, "G8", piecesTexture);
        board.addPieceToBoard("rook", ROOK, PIECE_BLACK, "H8", piecesTexture);
        // TESTING

        // board.addPieceToBoard("king", KING, PIECE_WHITE, "A1", piecesTexture);
        // board.whiteKing = &pieceList->back();
        // board.addPieceToBoard("bishop", BISHOP, PIECE_WHITE, "F4", piecesTexture);
        // board.addPieceToBoard("knight", KNIGHT, PIECE_WHITE, "F3", piecesTexture);
        // board.addPieceToBoard("king", KING, PIECE_BLACK, "F6", piecesTexture);
        // board.blackKing = &pieceList->back();
    }

    Piece *currentPiece = nullptr;
    for (auto &p: *pieceList) {
        p.calculateLegalMoves(&board);
    }

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
            for (auto &p: *pieceList) {
                if (p.taken) continue;
                if (turn != p.colour) continue;
                if (CheckCollisionPointRec(mouse, p.boundingBox)) {
                    p.isCurrentlyHeld = true;
                    currentPiece = &p;
                    currentPiece->lastPosition = currentPiece->getCurrentPos();
                    break;
                }
            }
        }
        // Move held piece with cursor
        for (auto &p: *pieceList) {
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
        }

        // Draw held piece and its legal moves
        if (currentPiece != nullptr) {
            for (std::string &legalMove: currentPiece->legalMoves) {
                board.drawLegalMove(legalMove, currentPiece->colour);
            }
            currentPiece->Draw(piecesTexture);
        }
        if (!turn)
            DrawText("White's turn", 10, 10, 20, WHITE);
        if (turn)
            DrawText("Black's turn", 10, 10, 20, WHITE);
        EndDrawing();
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
