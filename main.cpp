#include <algorithm>
#include <fstream>
#include <iostream>
#include <vector>
#include "Board.h"
#include "raylib.h"
#include "Piece.h"

auto TITLE = "Chess";

static bool turn = PIECE_WHITE;

void checkDropPosition(Piece *&currentPiece, Board &board) {
    if (currentPiece == nullptr) return;
    bool foundValidMove = false;
    for (auto &[notation, square]: board.squares) {
        if (CheckCollisionPointRec(GetMousePosition(), square.squareBox)) {
            if (notation == currentPiece->square->name) break;
            if (!currentPiece->isLegalMove(notation)) {
                std::cout << notation + " not legal \n";
                currentPiece->setCurrentPos({currentPiece->lastPosition.x, currentPiece->lastPosition.y});
                break;
            }

            if (square.piece && !square.piece->taken) {
                if (square.piece->colour == currentPiece->colour) {
                    std::cout << notation + " invalid \n";
                    currentPiece->setCurrentPos({currentPiece->lastPosition.x, currentPiece->lastPosition.y});
                    break;
                }
                square.piece->taken = true;
                std::cout << "taken ";
            }
            currentPiece->setCurrentPos({square.squareBox.x, square.squareBox.y});
            currentPiece->lastPosition = currentPiece->getCurrentPos();
            currentPiece->square->piece = nullptr;
            square.piece = currentPiece;
            currentPiece->square = &square;
            std::cout << notation + " valid \n";
            if (!currentPiece->hasMoved) currentPiece->hasMoved = true;
            turn = !turn;
            board.calculateAllLegalMoves();
            foundValidMove = true;
            break;
        }
    }
    if (!foundValidMove) {
        currentPiece->setCurrentPos({currentPiece->lastPosition.x, currentPiece->lastPosition.y});
    } else {
        currentPiece = nullptr;
    }
}

int main() {
    InitWindow(750, 750, TITLE);

    const Texture2D piecesTexture = LoadTexture("../Chess_Pieces_Sprite.png");

    Board board;
    std::vector<Piece>* pieceList = &board.pieceList;
    pieceList->reserve(32);
    {
        // White Pieces
        pieceList->emplace_back("pawn", PAWN, PIECE_WHITE, board.squares["A2"], piecesTexture);
        board.squares["A2"].piece = &pieceList->back();
        pieceList->emplace_back("pawn", PAWN, PIECE_WHITE, board.squares["B2"], piecesTexture);
        board.squares["B2"].piece = &pieceList->back();
        pieceList->emplace_back("pawn", PAWN, PIECE_WHITE, board.squares["C2"], piecesTexture);
        board.squares["C2"].piece = &pieceList->back();
        pieceList->emplace_back("pawn", PAWN, PIECE_WHITE, board.squares["D2"], piecesTexture);
        board.squares["D2"].piece = &pieceList->back();
        pieceList->emplace_back("pawn", PAWN, PIECE_WHITE, board.squares["E2"], piecesTexture);
        board.squares["E2"].piece = &pieceList->back();
        pieceList->emplace_back("pawn", PAWN, PIECE_WHITE, board.squares["F2"], piecesTexture);
        board.squares["F2"].piece = &pieceList->back();
        pieceList->emplace_back("pawn", PAWN, PIECE_WHITE, board.squares["G2"], piecesTexture);
        board.squares["G2"].piece = &pieceList->back();
        pieceList->emplace_back("pawn", PAWN, PIECE_WHITE, board.squares["H2"], piecesTexture);
        board.squares["H2"].piece = &pieceList->back();
        pieceList->emplace_back("rook", ROOK, PIECE_WHITE, board.squares["A1"], piecesTexture);
        board.squares["A1"].piece = &pieceList->back();
        pieceList->emplace_back("knight", KNIGHT, PIECE_WHITE, board.squares["B1"], piecesTexture);
        board.squares["B1"].piece = &pieceList->back();
        pieceList->emplace_back("bishop", BISHOP, PIECE_WHITE, board.squares["C1"], piecesTexture);
        board.squares["C1"].piece = &pieceList->back();
        pieceList->emplace_back("queen", QUEEN, PIECE_WHITE, board.squares["D1"], piecesTexture);
        board.squares["D1"].piece = &pieceList->back();
        pieceList->emplace_back("king", KING, PIECE_WHITE, board.squares["E1"], piecesTexture);
        board.squares["E1"].piece = &pieceList->back();
        board.whiteKing = &pieceList->back();
        pieceList->emplace_back("bishop", BISHOP, PIECE_WHITE, board.squares["F1"], piecesTexture);
        board.squares["F1"].piece = &pieceList->back();
        pieceList->emplace_back("knight", KNIGHT, PIECE_WHITE, board.squares["G1"], piecesTexture);
        board.squares["G1"].piece = &pieceList->back();
        pieceList->emplace_back("rook", ROOK, PIECE_WHITE, board.squares["H1"], piecesTexture);
        board.squares["H1"].piece = &pieceList->back();
        // Black Pieces
        pieceList->emplace_back("pawn", PAWN, PIECE_BLACK, board.squares["A7"], piecesTexture);
        board.squares["A7"].piece = &pieceList->back();
        pieceList->emplace_back("pawn", PAWN, PIECE_BLACK, board.squares["B7"], piecesTexture);
        board.squares["B7"].piece = &pieceList->back();
        pieceList->emplace_back("pawn", PAWN, PIECE_BLACK, board.squares["C7"], piecesTexture);
        board.squares["C7"].piece = &pieceList->back();
        pieceList->emplace_back("pawn", PAWN, PIECE_BLACK, board.squares["D7"], piecesTexture);
        board.squares["D7"].piece = &pieceList->back();
        pieceList->emplace_back("pawn", PAWN, PIECE_BLACK, board.squares["E7"], piecesTexture);
        board.squares["E7"].piece = &pieceList->back();
        pieceList->emplace_back("pawn", PAWN, PIECE_BLACK, board.squares["F7"], piecesTexture);
        board.squares["F7"].piece = &pieceList->back();
        pieceList->emplace_back("pawn", PAWN, PIECE_BLACK, board.squares["G7"], piecesTexture);
        board.squares["G7"].piece = &pieceList->back();
        pieceList->emplace_back("pawn", PAWN, PIECE_BLACK, board.squares["H7"], piecesTexture);
        board.squares["H7"].piece = &pieceList->back();
        pieceList->emplace_back("rook", ROOK, PIECE_BLACK, board.squares["A8"], piecesTexture);
        board.squares["A8"].piece = &pieceList->back();
        pieceList->emplace_back("knight", KNIGHT, PIECE_BLACK, board.squares["B8"], piecesTexture);
        board.squares["B8"].piece = &pieceList->back();
        pieceList->emplace_back("bishop", BISHOP, PIECE_BLACK, board.squares["C8"], piecesTexture);
        board.squares["C8"].piece = &pieceList->back();
        pieceList->emplace_back("queen", QUEEN, PIECE_BLACK, board.squares["D8"], piecesTexture);
        board.squares["D8"].piece = &pieceList->back();
        pieceList->emplace_back("king", KING, PIECE_BLACK, board.squares["E8"], piecesTexture);
        board.squares["E8"].piece = &pieceList->back();
        board.blackKing = &pieceList->back();
        pieceList->emplace_back("bishop", BISHOP, PIECE_BLACK, board.squares["F8"], piecesTexture);
        board.squares["F8"].piece = &pieceList->back();
        pieceList->emplace_back("knight", KNIGHT, PIECE_BLACK, board.squares["G8"], piecesTexture);
        board.squares["G8"].piece = &pieceList->back();
        pieceList->emplace_back("rook", ROOK, PIECE_BLACK, board.squares["H8"], piecesTexture);
        board.squares["H8"].piece = &pieceList->back();
    }

    Piece *currentPiece = nullptr;
    for (auto &p: *pieceList) {
        p.calculateLegalMoves(*pieceList, &board);
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
        if (currentPiece != nullptr) {
            currentPiece->Draw(piecesTexture);
            for (std::string &legalMove: currentPiece->legalMoves) {
                board.drawLegalMove(legalMove, currentPiece->colour);
            }
        }

        if (!turn)
            DrawText("White's turn", 10, 10, 20,WHITE);
        if (turn)
            DrawText("Black's turn", 10, 10, 20,WHITE);
        EndDrawing();
    }
    UnloadTexture(piecesTexture);
    CloseWindow();
    return 0;
}
