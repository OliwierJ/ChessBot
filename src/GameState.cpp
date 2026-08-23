//
// Created by papie on 04/05/2026.
//

#include "GameState.h"

void GameState::reset_state() {
    turn = PieceColor::White;
    state = GameStatus::Normal;
    winner = std::nullopt;
    move_history.clear_history();
    evaluation = 0;
}
