//
// Created by papie on 19/08/2026.
//

#ifndef CHESSBOT_MOVEVALIDATOR_H
#define CHESSBOT_MOVEVALIDATOR_H
#include "GameState.h"
#include "Piece.h"


class MoveValidator {
public:
    static bool validate_legal_move(Piece *currentPiece, BoardSquare target_square, Board board, GameState state);
};


#endif //CHESSBOT_MOVEVALIDATOR_H