//
// Created by papie on 19/08/2026.
//

#include "MoveValidator.h"

#include "GameState.h"
#include "Piece.h"

bool MoveValidator::validate_legal_move(Piece* currentPiece, BoardSquare target_square, Board board, GameState state) {

    if (target_square.name == currentPiece->square->name) return false;
    if (!currentPiece->isLegalMove(target_square.name)) {
        currentPiece->setCurrentPos({currentPiece->lastPosition.x, currentPiece->lastPosition.y});
        return false;
    }

    // Valid that the move doesnt keep the player in check
    if (board.isColourChecked(currentPiece->colour)) {
        std::string takenSquare;
        Piece *enpassantPiece = nullptr;
        if (std::ranges::count(board.enpassantSquares, target_square.name)) {
            const int upOrDownMove = currentPiece->colour == PIECE_WHITE ? -1 : 1;
            takenSquare = {(target_square.name[0]), static_cast<char>(target_square.name[1] + upOrDownMove)};
            enpassantPiece = board.squares[takenSquare].piece;
            board.squares[takenSquare].piece->taken = true;
            board.squares[takenSquare].piece = nullptr;
        }
        const auto tempSquare = currentPiece->square;
        const auto tempCurrent = target_square.piece;
        currentPiece->square = &target_square;
        target_square.piece = currentPiece;
        if (tempCurrent != nullptr) tempCurrent->taken = true;

        board.calculateAllLegalMoves();
        auto newAttackedSquares = board.attackedSquaresOfColor(currentPiece->colour);
        if (currentPiece->colour == PIECE_WHITE) {
            board.whiteIsChecked = std::ranges::count(newAttackedSquares, board.whiteKing->square->name) >= 1;
            if (board.whiteIsChecked) {
                if (tempCurrent != nullptr) tempCurrent->taken = false;
                currentPiece->square = tempSquare;
                target_square.piece = tempCurrent;

                state.illegalMove = true;
                return false;
            }
        } else {
            board.blackIsChecked = std::ranges::count(newAttackedSquares, board.blackKing->square->name) >= 1;
            if (board.blackIsChecked) {
                if (tempCurrent != nullptr) tempCurrent->taken = false;
                currentPiece->square = tempSquare;
                target_square.piece = tempCurrent;
                state.illegalMove = true;

                return false;
            }
        }
        if (tempCurrent != nullptr) tempCurrent->taken = false;
        currentPiece->square = tempSquare;
        target_square.piece = tempCurrent;
        if (enpassantPiece != nullptr) {
            board.squares[takenSquare].piece = enpassantPiece;
        }
    }

    if (target_square.piece && !target_square.piece->taken) {
        if (target_square.piece->colour == currentPiece->colour) {
            currentPiece->setCurrentPos({currentPiece->lastPosition.x, currentPiece->lastPosition.y});
            return false;
        }
        state.pieceTaken = true;
        target_square.piece->taken = true;
    }
    return true;
}
