#include <algorithm>
#include <fstream>
#include <iostream>
#include <vector>
#include "Board.h"
#include "GameState.h"
#include "raylib.h"
#include "Piece.h"

auto TITLE = "Chess";

static bool turn = PIECE_WHITE;

Sound checkSound;
Sound moveSound;
Sound illegalMoveSound;
Sound captureSound;
Sound promoteSound;
Sound gameEndSound;

void checkDropPosition(Piece *&currentPiece, Board &board, GameState &state) {
    if (currentPiece == nullptr) return;
    bool foundValidMove = false;
    bool pieceTaken = false;
    bool pawnPromoted = false;

    for (auto &[notation, square]: board.squares) {
        if (CheckCollisionPointRec(GetMousePosition(), square.squareBox)) {
            if (notation == currentPiece->square->name) break;
            if (!currentPiece->isLegalMove(notation)) {
                currentPiece->setCurrentPos({currentPiece->lastPosition.x, currentPiece->lastPosition.y});
                break;
            }

            if (board.isColourChecked(currentPiece->colour)) {
                std::string takenSquare;
                Piece* enpassantPiece = nullptr;
                if (std::ranges::count(board.enpassantSquares, square.name)) {
                    const int upOrDownMove = currentPiece->colour == PIECE_WHITE ? -1 : 1;
                    takenSquare = {(square.name[0]), static_cast<char>(square.name[1] + upOrDownMove)};
                    enpassantPiece = board.squares[takenSquare].piece;
                    board.squares[takenSquare].piece->taken = true;
                    board.squares[takenSquare].piece = nullptr;
                }
                const auto tempSquare = currentPiece->square;
                const auto tempCurrent = square.piece;
                currentPiece->square = &square;
                square.piece = currentPiece;
                if (tempCurrent != nullptr) tempCurrent->taken = true;

                board.calculateAllLegalMoves();
                auto newAttackedSquares = board.attackedSquaresOfColor(currentPiece->colour);
                if (currentPiece->colour == PIECE_WHITE) {
                    board.whiteIsChecked = std::ranges::count(newAttackedSquares, board.whiteKing->square->name) >= 1;
                    if (board.whiteIsChecked) {
                        if (tempCurrent != nullptr) tempCurrent->taken = false;
                        currentPiece->square = tempSquare;
                        square.piece = tempCurrent;

                        std::cout << "white still checked " << board.whiteIsChecked << "\n";
                        PlaySound(illegalMoveSound);
                        break;
                    }
                    std::cout << "white no longer checked " << board.whiteIsChecked << "\n";
                } else {
                    board.blackIsChecked = std::ranges::count(newAttackedSquares, board.blackKing->square->name) >= 1;
                    if (board.blackIsChecked) {
                        if (tempCurrent != nullptr) tempCurrent->taken = false;
                        currentPiece->square = tempSquare;
                        square.piece = tempCurrent;
                        std::cout << "black still checked " << board.blackIsChecked << "\n";
                        PlaySound(illegalMoveSound);
                        break;
                    }
                    std::cout << "black no longer checked " << board.blackIsChecked << "\n";
                }
                if (tempCurrent != nullptr) tempCurrent->taken = false;
                currentPiece->square = tempSquare;
                square.piece = tempCurrent;
                if (enpassantPiece != nullptr) {
                    board.squares[takenSquare].piece = enpassantPiece;
                }
            }

            if (square.piece && !square.piece->taken) {
                if (square.piece->colour == currentPiece->colour) {
                    currentPiece->setCurrentPos({currentPiece->lastPosition.x, currentPiece->lastPosition.y});
                    break;
                }
                pieceTaken = true;
                square.piece->taken = true;
                std::cout << "taken ";
            }

            if (std::ranges::count(board.enpassantSquares, square.name)) {
                const int upOrDownMove = currentPiece->colour == PIECE_WHITE ? -1 : 1;
                std::string takenSquare = {(square.name[0]), static_cast<char>(square.name[1] + upOrDownMove)};

                if (board.squares[takenSquare].piece->colour == currentPiece->colour) {
                    currentPiece->setCurrentPos({currentPiece->lastPosition.x, currentPiece->lastPosition.y});
                    break;
                }
                board.squares[takenSquare].piece->taken = true;
                board.squares[takenSquare].piece = nullptr;
                pieceTaken = true;
                std::cout << "taken " << takenSquare;
            }
            board.enpassantSquares.clear();

            if (!currentPiece->hasMoved) {
                currentPiece->hasMoved = true;
                if (currentPiece->type == "king") {
                    if (currentPiece->colour == PIECE_WHITE) {
                        if (square.name == "G1") {
                            const auto rRook = board.squares["H1"].piece;
                            rRook->setCurrentPos({board.squares["F1"].squareBox.x, board.squares["F1"].squareBox.y});
                            rRook->lastPosition = currentPiece->getCurrentPos();
                            rRook->square->piece = nullptr;
                            board.squares["F1"].piece = rRook;
                            rRook->square = &board.squares["F1"];
                        }
                        if (square.name == "C1") {
                            const auto lRook = board.squares["A1"].piece;
                            lRook->setCurrentPos({board.squares["D1"].squareBox.x, board.squares["D1"].squareBox.y});
                            lRook->lastPosition = currentPiece->getCurrentPos();
                            lRook->square->piece = nullptr;
                            board.squares["D1"].piece = lRook;
                            lRook->square = &board.squares["D1"];
                        }
                    } else {
                        if (square.name == "G8") {
                            const auto rRook = board.squares["H8"].piece;
                            rRook->setCurrentPos({board.squares["F8"].squareBox.x, board.squares["F8"].squareBox.y});
                            rRook->lastPosition = currentPiece->getCurrentPos();
                            rRook->square->piece = nullptr;
                            board.squares["F8"].piece = rRook;
                            rRook->square = &board.squares["F8"];
                        }
                        if (square.name == "C8") {
                            const auto lRook = board.squares["A8"].piece;
                            lRook->setCurrentPos({board.squares["D8"].squareBox.x, board.squares["D8"].squareBox.y});
                            lRook->lastPosition = currentPiece->getCurrentPos();
                            lRook->square->piece = nullptr;
                            board.squares["D8"].piece = lRook;
                            lRook->square = &board.squares["D8"];
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
                        if (square.name[1] == '4') {
                            board.enpassantSquares.push_back({square.name[0], '3'});
                        }
                    } else {
                        if (square.name[1] == '5') {
                            board.enpassantSquares.push_back({square.name[0], '6'});
                        }
                    }
                }
            }
            currentPiece->setCurrentPos({square.squareBox.x, square.squareBox.y});
            currentPiece->lastPosition = currentPiece->getCurrentPos();
            currentPiece->square->piece = nullptr;
            square.piece = currentPiece;
            currentPiece->square = &square;
            std::cout << notation + " valid \n";
            pawnPromoted = currentPiece->try_promote();
            board.calculateAllLegalMovesByColour(!turn);
            if (currentPiece->colour == PIECE_WHITE) {
                auto temp = board.attackedSquaresOfColor(PIECE_BLACK);
                board.blackIsChecked = std::ranges::count(temp, board.blackKing->square->name) >= 1;
                std::cout << "is black checked? " << board.blackIsChecked << "\n";
            } else {
                auto temp = board.attackedSquaresOfColor(PIECE_WHITE);
                board.whiteIsChecked = std::ranges::count(temp, board.whiteKing->square->name) >= 1;
                std::cout << "is white checked? " << board.whiteIsChecked << "\n";
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

            foundValidMove = true;
            turn = !turn;
            break;
        }
    }

    if (!foundValidMove) {
        currentPiece->setCurrentPos({currentPiece->lastPosition.x, currentPiece->lastPosition.y});
        board.calculateAllLegalMovesByColour(turn);
    } else {
        currentPiece = nullptr;
    }
}

void DrawEndGameState(GameState game, const Board &board, const Texture2D &piecesTexture) {
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

int main() {
    InitWindow(750, 750, TITLE);
    std::cout << std::boolalpha;
    InitAudioDevice();
    checkSound = LoadSound("../resources/move-check.wav");
    moveSound = LoadSound("../resources/move-self.wav");
    illegalMoveSound = LoadSound("../resources/illegal.wav");
    captureSound = LoadSound("../resources/capture.mp3");
    promoteSound = LoadSound("../resources/promote.mp3");
    gameEndSound = LoadSound("../resources/game-end.mp3");
    const Texture2D piecesTexture = LoadTexture("../resources/Chess_Pieces_Sprite.png");

    GameState game;
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
        // // Black Pieces
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
        // board.addPieceToBoard("pawn", PAWN, PIECE_WHITE, "A7", piecesTexture);
        // board.addPieceToBoard("pawn", PAWN, PIECE_WHITE, "B5", piecesTexture);
        // board.addPieceToBoard("pawn", PAWN, PIECE_WHITE, "C2", piecesTexture);
        // board.addPieceToBoard("pawn", PAWN, PIECE_WHITE, "D2", piecesTexture);
        // board.addPieceToBoard("pawn", PAWN, PIECE_WHITE, "E2", piecesTexture);
        // board.addPieceToBoard("pawn", PAWN, PIECE_WHITE, "F2", piecesTexture);
        // board.addPieceToBoard("pawn", PAWN, PIECE_WHITE, "G2", piecesTexture);
        // board.addPieceToBoard("pawn", PAWN, PIECE_WHITE, "H2", piecesTexture);
        // board.addPieceToBoard("rook", ROOK, PIECE_WHITE, "A1", piecesTexture);
        // board.addPieceToBoard("knight", KNIGHT, PIECE_WHITE, "B1", piecesTexture);
        // board.addPieceToBoard("bishop", BISHOP, PIECE_WHITE, "C1", piecesTexture);
        // board.addPieceToBoard("queen", QUEEN, PIECE_WHITE, "A1", piecesTexture);
        // board.addPieceToBoard("king", KING, PIECE_WHITE, "E4", piecesTexture);
        // board.whiteKing = &pieceList->back();
        // board.addPieceToBoard("bishop", BISHOP, PIECE_WHITE, "F1", piecesTexture);
        // board.addPieceToBoard("knight", KNIGHT, PIECE_WHITE, "G1", piecesTexture);
        // board.addPieceToBoard("rook", ROOK, PIECE_WHITE, "H1", piecesTexture);
        // Black Pieces
        // board.addPieceToBoard("pawn", PAWN, PIECE_BLACK, "A2", piecesTexture);
        // board.addPieceToBoard("queen", QUEEN, PIECE_BLACK, "A7", piecesTexture);
        // board.addPieceToBoard("pawn", PAWN, PIECE_BLACK, "C7", piecesTexture);
        // board.addPieceToBoard("pawn", PAWN, PIECE_BLACK, "D6", piecesTexture);
        // board.addPieceToBoard("pawn", PAWN, PIECE_BLACK, "E7", piecesTexture);
        // board.addPieceToBoard("queen", QUEEN, PIECE_BLACK, "F7", piecesTexture);
        // board.addPieceToBoard("queen", QUEEN, PIECE_BLACK, "G7", piecesTexture);
        // board.addPieceToBoard("queen", QUEEN, PIECE_BLACK, "H7", piecesTexture);
        // board.addPieceToBoard("rook", ROOK, PIECE_BLACK, "A8", piecesTexture);
        // board.addPieceToBoard("knight", KNIGHT, PIECE_BLACK, "B8", piecesTexture);
        // board.addPieceToBoard("bishop", BISHOP, PIECE_BLACK, "C8", piecesTexture);
        // board.addPieceToBoard("queen", QUEEN, PIECE_BLACK, "D8", piecesTexture);
        // board.addPieceToBoard("king", KING, PIECE_BLACK, "E8", piecesTexture);
        // board.blackKing = &pieceList->back();
        // board.addPieceToBoard("bishop", BISHOP, PIECE_BLACK, "F8", piecesTexture);
        // board.addPieceToBoard("knight", KNIGHT, PIECE_BLACK, "G8", piecesTexture);
        // board.addPieceToBoard("rook", ROOK, PIECE_BLACK, "H8", piecesTexture);
    }

    Piece *currentPiece = nullptr;
    for (auto &p: *pieceList) {
        p.calculateLegalMoves(&board);
    }

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(DARKBROWN);
        Board::Draw();
        DrawRectangle(800, 65, 200, 675, LIGHTGRAY);

        if (game.state == CHECKMATE || game.state == STALEMATE) {
            DrawEndGameState(game, board, piecesTexture);
            EndDrawing();
            continue;
        }
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
            checkDropPosition(currentPiece, board, game);
        }

        if (currentPiece != nullptr) {
            currentPiece->Draw(piecesTexture);
            for (std::string &legalMove: currentPiece->legalMoves) {
                board.drawLegalMove(legalMove, currentPiece->colour);
            }
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
