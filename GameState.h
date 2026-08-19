//
// Created by papie on 04/05/2026.
//

#ifndef CHESSBOT_GAMESTATE_H
#define CHESSBOT_GAMESTATE_H
#include "MoveHistory.h"

enum STATE {NORMAL, CHECKMATE, STALEMATE};

class GameState {
public:
    int state = NORMAL;
    int winner = -1;
    MoveHistory move_history;
    bool pieceTaken = false;
    bool pawnPromoted = false;
    bool shortCastled = false;
    bool longCastled = false;
    bool illegalMove = false;
    // more states idk
};


#endif //CHESSBOT_GAMESTATE_H