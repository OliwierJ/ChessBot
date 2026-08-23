//
// Created by papie on 04/05/2026.
//

#ifndef CHESSBOT_GAMESTATE_H
#define CHESSBOT_GAMESTATE_H
#include "MoveHistory.h"
#include <optional>
#include "Board.h"

enum class GameStatus {Menu, Normal, Checkmate, Stalemate};

class GameState {
public:
    PieceColor turn = PieceColor::White;
    GameStatus state = GameStatus::Menu;
    std::optional<PieceColor> winner;
    bool bot_game = true;
    MoveHistory move_history;

    void reset_state();

};


#endif //CHESSBOT_GAMESTATE_H