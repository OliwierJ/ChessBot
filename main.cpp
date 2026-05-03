#include <algorithm>
#include <fstream>
#include <iostream>
#include <vector>
#include "Board.h"
#include "raylib.h"
#include "Piece.h"

auto TITLE = "Chess";

static bool turn = PIECE_WHITE;

Sound checkSound;
Sound moveSound;
Sound illegalMoveSound;
Sound captureSound;

void checkDropPosition(Piece *&currentPiece, Board &board) {
    if (currentPiece == nullptr) return;
    bool foundValidMove = false;
    for (auto &[notation, square]: board.squares) {
        if (CheckCollisionPointRec(GetMousePosition(), square.squareBox)) {
            if (notation == currentPiece->square->name) break;
            if (!currentPiece->isLegalMove(notation)) {
                currentPiece->setCurrentPos({currentPiece->lastPosition.x, currentPiece->lastPosition.y});
                break;
            }

            if (board.isColourChecked(currentPiece->colour)) {
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
            }

            if (square.piece && !square.piece->taken) {
                if (square.piece->colour == currentPiece->colour) {
                    currentPiece->setCurrentPos({currentPiece->lastPosition.x, currentPiece->lastPosition.y});
                    break;
                }
                square.piece->taken = true;
                PlaySound(captureSound);
                std::cout << "taken ";
            }

            currentPiece->setCurrentPos({square.squareBox.x, square.squareBox.y});
            currentPiece->lastPosition = currentPiece->getCurrentPos();
            currentPiece->square->piece = nullptr;
            square.piece = currentPiece;
            currentPiece->square = &square;
            std::cout << notation + " valid \n";
            if (!currentPiece->hasMoved) currentPiece->hasMoved = true;
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


            if (board.whiteIsChecked || board.blackIsChecked) {
                PlaySound(checkSound);
            }
            PlaySound(moveSound);

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

int main() {
    InitWindow(750, 750, TITLE);
    std::cout << std::boolalpha;
    InitAudioDevice();
    checkSound = LoadSound("../resources/move-check.wav");
    moveSound = LoadSound("../resources/move-self.wav");
    illegalMoveSound = LoadSound("../resources/illegal.wav");
    captureSound = LoadSound("../resources/capture.mp3");
    const Texture2D piecesTexture = LoadTexture("../resources/Chess_Pieces_Sprite.png");


    Board board;
    std::vector<Piece>* pieceList = &board.pieceList;
    pieceList->reserve(32);
    {
        // White Pieces
        board.addPieceToBoard("pawn", PAWN, PIECE_WHITE, "A2", piecesTexture);
        board.addPieceToBoard("pawn", PAWN, PIECE_WHITE, "B2", piecesTexture);
        board.addPieceToBoard("pawn", PAWN, PIECE_WHITE, "C2", piecesTexture);
        board.addPieceToBoard("pawn", PAWN, PIECE_WHITE, "D2", piecesTexture);
        board.addPieceToBoard("pawn", PAWN, PIECE_WHITE, "E2", piecesTexture);
        // board.addPieceToBoard("pawn", PAWN, PIECE_WHITE, "F2", piecesTexture);
        // board.addPieceToBoard("pawn", PAWN, PIECE_WHITE, "G2", piecesTexture);
        // board.addPieceToBoard("pawn", PAWN, PIECE_WHITE, "H2", piecesTexture);
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
        // board.addPieceToBoard("pawn", PAWN, PIECE_BLACK, "A7", piecesTexture);
        // board.addPieceToBoard("pawn", PAWN, PIECE_BLACK, "B7", piecesTexture);
        // board.addPieceToBoard("pawn", PAWN, PIECE_BLACK, "C7", piecesTexture);
        // board.addPieceToBoard("pawn", PAWN, PIECE_BLACK, "D7", piecesTexture);
        board.addPieceToBoard("pawn", PAWN, PIECE_BLACK, "E7", piecesTexture);
        board.addPieceToBoard("pawn", PAWN, PIECE_BLACK, "F7", piecesTexture);
        board.addPieceToBoard("pawn", PAWN, PIECE_BLACK, "G7", piecesTexture);
        board.addPieceToBoard("pawn", PAWN, PIECE_BLACK, "H7", piecesTexture);
        // board.addPieceToBoard("rook", ROOK, PIECE_BLACK, "A8", piecesTexture);
        // board.addPieceToBoard("knight", KNIGHT, PIECE_BLACK, "B8", piecesTexture);
        // board.addPieceToBoard("bishop", BISHOP, PIECE_BLACK, "C8", piecesTexture);
        board.addPieceToBoard("queen", QUEEN, PIECE_BLACK, "D8", piecesTexture);
        board.addPieceToBoard("king", KING, PIECE_BLACK, "E8", piecesTexture);
        board.blackKing = &pieceList->back();
        board.addPieceToBoard("bishop", BISHOP, PIECE_BLACK, "F8", piecesTexture);
        board.addPieceToBoard("knight", KNIGHT, PIECE_BLACK, "G8", piecesTexture);
        board.addPieceToBoard("rook", ROOK, PIECE_BLACK, "H8", piecesTexture);
    }

    Piece *currentPiece = nullptr;
    for (auto &p: *pieceList) {
        p.calculateLegalMoves(&board);
    }
    // board.pieceList = &pieceList;

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(DARKBROWN);
        Board::Draw();

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
            checkDropPosition(currentPiece, board);
        }
        // if (currentPiece) {
        //     std::cout << currentPiece->type << "  " << currentPiece->colour << "  ";
        //     currentPiece->printLegalMoves();
        // }
        if (currentPiece != nullptr) {
            currentPiece->Draw(piecesTexture);
            for (std::string &legalMove : currentPiece->legalMoves) {

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
    CloseAudioDevice();
    CloseWindow();
    return 0;
}
