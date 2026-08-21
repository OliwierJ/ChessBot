//
// Created by papie on 04/05/2026.
//

#ifndef CHESSBOT_GAMESTATE_H
#define CHESSBOT_GAMESTATE_H
#include "MoveHistory.h"
#include "Board.h"

enum STATE {NORMAL, CHECKMATE, STALEMATE};

class GameState {
public:
    int turn = 0;
    int state = NORMAL;
    int winner = -1;
    bool bot_game = true;
    Board* board;
    MoveHistory move_history;
    bool pieceTaken = false;
    bool pawnPromoted = false;
    bool shortCastled = false;
    bool longCastled = false;
    bool illegalMove = false;
};


#endif //CHESSBOT_GAMESTATE_H