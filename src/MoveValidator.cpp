//
// Created by papie on 19/08/2026.
//

#include "MoveValidator.h"
#include "Piece.h"
#include "Board.h"

#pragma region private helpers

void try_castle_king(const Piece& currentPiece, Board &board, const BoardSquare &square, MoveOutcome &move_outcome) {
    if (currentPiece.colour == PieceColor::White) {
        if (square.name == "G1") {
            const auto rRook = board.squares["H1"].piece;
            rRook->setCurrentPos({board.squares["F1"].squareBox.x, board.squares["F1"].squareBox.y});
            rRook->lastPosition = currentPiece.getCurrentPos();
            rRook->square->piece = nullptr;
            board.squares["F1"].piece = rRook;
            rRook->square = &board.squares["F1"];
            move_outcome.shortCastled = true;
        }
        if (square.name == "C1") {
            const auto lRook = board.squares["A1"].piece;
            lRook->setCurrentPos({board.squares["D1"].squareBox.x, board.squares["D1"].squareBox.y});
            lRook->lastPosition = currentPiece.getCurrentPos();
            lRook->square->piece = nullptr;
            board.squares["D1"].piece = lRook;
            lRook->square = &board.squares["D1"];
            move_outcome.longCastled = true;
        }
    } else {
        if (square.name == "G8") {
            const auto rRook = board.squares["H8"].piece;
            rRook->setCurrentPos({board.squares["F8"].squareBox.x, board.squares["F8"].squareBox.y});
            rRook->lastPosition = currentPiece.getCurrentPos();
            rRook->square->piece = nullptr;
            board.squares["F8"].piece = rRook;
            rRook->square = &board.squares["F8"];
            move_outcome.shortCastled = true;
        }
        if (square.name == "C8") {
            const auto lRook = board.squares["A8"].piece;
            lRook->setCurrentPos({board.squares["D8"].squareBox.x, board.squares["D8"].squareBox.y});
            lRook->lastPosition = currentPiece.getCurrentPos();
            lRook->square->piece = nullptr;
            board.squares["D8"].piece = lRook;
            lRook->square = &board.squares["D8"];
            move_outcome.longCastled = true;
        }
    }
}

void deny_castling_for_rook(const Piece& currentPiece, Board &board) {
    if (currentPiece.colour == PieceColor::White) {
        if (currentPiece.square->name == "A1") {
            board.whiteCanLongCastle = false;
        }
        if (currentPiece.square->name == "H1") {
            board.whiteCanShortCastle = false;
        }
    } else {
        if (currentPiece.square->name == "A8") {
            board.blackCanLongCastle = false;
        }
        if (currentPiece.square->name == "H8") {
            board.blackCanShortCastle = false;
        }
    }
}

void mark_square_as_enpassant(const Piece& currentPiece, Board &board, BoardSquare &square) {
    if (currentPiece.colour == PieceColor::White) {
        if (square.name[1] == '4') {
            board.enpassantSquares.push_back({square.name[0], '3'});
        }
    } else {
        if (square.name[1] == '5') {
            board.enpassantSquares.push_back({square.name[0], '6'});
        }
    }
}

#pragma endregion

bool MoveValidator::validate_legal_move(Piece& currentPiece, BoardSquare &target_square, Board &board) {
    if (target_square.name == currentPiece.square->name) return false;
    if (!currentPiece.isLegalMove(target_square.name)) return false;

    // Valid that the move doesn't keep the player in check
    if (board.isColourChecked(currentPiece.colour)) {
        std::string takenSquare;
        Piece *enpassantPiece = nullptr;
        if (std::ranges::count(board.enpassantSquares, target_square.name)) {
            const int upOrDownMove = currentPiece.colour == PieceColor::White ? -1 : 1;
            takenSquare = {(target_square.name[0]), static_cast<char>(target_square.name[1] + upOrDownMove)};
            enpassantPiece = board.squares[takenSquare].piece;
            board.squares[takenSquare].piece->captured = true;
            board.squares[takenSquare].piece = nullptr;
        }
        const auto tempSquare = currentPiece.square;
        const auto tempCurrent = target_square.piece;
        currentPiece.square = &target_square;
        target_square.piece = &currentPiece;
        if (tempCurrent != nullptr) tempCurrent->captured = true;

        board.calculateAllLegalMoves();
        auto newAttackedSquares = board.attackedSquaresOfColor(currentPiece.colour);
        if (currentPiece.colour == PieceColor::White) {
            board.whiteIsChecked = std::ranges::count(newAttackedSquares, board.whiteKing->square->name) >= 1;
            if (board.whiteIsChecked) {
                if (tempCurrent != nullptr) tempCurrent->captured = false;
                currentPiece.square = tempSquare;
                target_square.piece = tempCurrent;

                return false;
            }
        } else {
            board.blackIsChecked = std::ranges::count(newAttackedSquares, board.blackKing->square->name) >= 1;
            if (board.blackIsChecked) {
                if (tempCurrent != nullptr) tempCurrent->captured = false;
                currentPiece.square = tempSquare;
                target_square.piece = tempCurrent;

                return false;
            }
        }
        if (tempCurrent != nullptr) tempCurrent->captured = false;
        currentPiece.square = tempSquare;
        target_square.piece = tempCurrent;
        if (enpassantPiece != nullptr) {
            board.squares[takenSquare].piece = enpassantPiece;
        }
    }

    if (target_square.piece && !target_square.piece->captured && target_square.piece->colour == currentPiece.colour) {
        return false;
    }
    return true;
}

MoveOutcome MoveValidator::apply_move(Piece& currentPiece, Board& board, BoardSquare &square) {
    MoveOutcome move_outcome;

    if (square.piece && !square.piece->captured) {
        move_outcome.pieceTaken = true;
        square.piece->captured = true;
    }

    // perform en passant
    if (std::ranges::count(board.enpassantSquares, square.name)) {
        const int upOrDownMove = currentPiece.colour == PieceColor::White ? -1 : 1;
        const std::string takenSquare = {(square.name[0]), static_cast<char>(square.name[1] + upOrDownMove)};

        board.squares[takenSquare].piece->captured = true;
        board.squares[takenSquare].piece = nullptr;
        move_outcome.pieceTaken = true;
    }
    board.enpassantSquares.clear();

    if (!currentPiece.hasMoved) {
        currentPiece.hasMoved = true;
        if (currentPiece.type == PieceType::King) {
            try_castle_king(currentPiece, board, square, move_outcome);
        }

        if (currentPiece.type == PieceType::Rook) {
            deny_castling_for_rook(currentPiece, board);
        }

        if (currentPiece.type == PieceType::Pawn) {
            mark_square_as_enpassant(currentPiece, board, square);
        }
    }
    // move piece
    currentPiece.setCurrentPos({square.squareBox.x, square.squareBox.y});
    currentPiece.lastPosition = currentPiece.getCurrentPos();
    currentPiece.square->piece = nullptr;
    square.piece = &currentPiece;
    currentPiece.square = &square;

    // attempt to promote
    move_outcome.pawnPromoted = currentPiece.try_promote();

    // calculate whether a check occurred
    board.calculateAllLegalMovesByColour(opposite(currentPiece.colour));
    if (currentPiece.colour == PieceColor::White) {
        auto temp = board.attackedSquaresOfColor(PieceColor::Black);
        board.blackIsChecked = std::ranges::count(temp, board.blackKing->square->name) >= 1;
        move_outcome.check = board.blackIsChecked;
    } else {
        auto temp = board.attackedSquaresOfColor(PieceColor::White);
        board.whiteIsChecked = std::ranges::count(temp, board.whiteKing->square->name) >= 1;
        move_outcome.check = board.whiteIsChecked;
    }
    move_outcome.validMove = true;
    return move_outcome;
}
