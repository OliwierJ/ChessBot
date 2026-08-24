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
    int turn_counter = 1;
    int last_pawn_or_capture = 1;
    bool bot_game = true;
    float evaluation = 0;
    MoveHistory move_history;
    std::optional<PieceColor> winner;
    bool white_castled = false;
    bool black_castled = false;

    void reset_state();

};


#endif //CHESSBOT_GAMESTATE_H