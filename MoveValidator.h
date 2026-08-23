//
// Created by papie on 19/08/2026.
//

#ifndef CHESSBOT_MOVEVALIDATOR_H
#define CHESSBOT_MOVEVALIDATOR_H

class Piece;
class Board;
class BoardSquare;

struct MoveOutcome {
    bool validMove = false;
    bool pieceTaken = false;
    bool pawnPromoted = false;
    bool shortCastled = false;
    bool longCastled = false;
    bool check = false;
};

class MoveValidator {
public:
    static bool validate_legal_move(Piece &currentPiece, BoardSquare &target_square, Board &board);

    static MoveOutcome apply_move(Piece &currentPiece, Board &board, BoardSquare &square);
};


#endif //CHESSBOT_MOVEVALIDATOR_H