//
// Created by papie on 04/05/2026.
//

#ifndef CHESSBOT_GAMESTATE_H
#define CHESSBOT_GAMESTATE_H

enum STATE {NORMAL, CHECKMATE, STALEMATE};

class GameState {
public:
    int state = NORMAL;
    int winner = -1;
    // more states idk
};


#endif //CHESSBOT_GAMESTATE_H