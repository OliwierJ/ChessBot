//
// Created by papie on 19/08/2026.
//

#ifndef CHESSBOT_MOVEVALIDATOR_H
#define CHESSBOT_MOVEVALIDATOR_H
#include "GameState.h"

class Piece;
class Board;
class BoardSquare;

struct MoveOutcome {
    bool pieceTaken = false;
    bool pawnPromoted = false;
    bool shortCastled = false;
    bool longCastled = false;
};

class MoveValidator {
public:
    static bool validate_legal_move(Piece *currentPiece, BoardSquare &target_square, Board &board);

    static void apply_move(Piece *&currentPiece, const GameState &game, BoardSquare &square, MoveOutcome &move_outcome);
};


#endif //CHESSBOT_MOVEVALIDATOR_H