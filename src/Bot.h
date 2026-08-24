//
// Created by papie on 23/08/2026.
//

#ifndef CHESSBOT_BOT_H
#define CHESSBOT_BOT_H
#include <optional>

#include "BoardSquare.h"
#include "ChessGame.h"
#include "Piece.h"

struct MoveOutcome;
class Piece;
class BoardSquare;

struct BotMove {
    std::string from;
    std::string target;
};


class Bot {
public:
    static std::vector<BotMove> get_legal_moves(PieceColor colour, const Board &board);

    static float calculate_position(const Board &board, const GameState &state);

    static std::optional<MoveOutcome> perform_bot_move(ChessGame &game);

    static std::optional<BotMove> choose_move(const Board &board, GameState &state, int depth, float alpha, float beta);

    static float minimax(const Board &board, const GameState &state, int depth, float alpha, float beta);
};


#endif //CHESSBOT_BOT_H