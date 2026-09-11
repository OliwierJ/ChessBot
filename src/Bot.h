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

constexpr int MAX_DEPTH = 4;

class Bot {
public:
    static std::vector<BotMove> get_legal_moves(PieceColor colour, const Board &board);

    static float calculate_position(const Board &board, const GameState &state);

    static std::optional<BotMove> choose_move(const Board &board, const GameState &state, int depth, float alpha, float beta);

private:
    struct MoveRecord {
        Piece *moved_piece = nullptr;
        BoardSquare *from_square = nullptr;
        BoardSquare *to_square = nullptr;

        bool moved_piece_prev_has_moved = false;
        PieceType moved_piece_prev_type = PieceType::Pawn;
        int moved_piece_prev_value = 1;
        Rectangle moved_piece_prev_texture{};
        Vector2 moved_piece_prev_pos{};
        Vector2 moved_piece_prev_last_pos{};

        Piece *captured_piece = nullptr;
        BoardSquare *captured_piece_square = nullptr;
        bool captured_piece_prev_captured = false;
        bool was_en_passant_capture = false;

        Piece *rook_piece = nullptr;
        BoardSquare *rook_from_square = nullptr;
        BoardSquare *rook_to_square = nullptr;
        bool rook_prev_has_moved = false;
        Vector2 rook_prev_pos{};
        Vector2 rook_prev_last_pos{};

        std::unordered_set<std::string> prev_en_passant_squares;
        bool prev_white_can_short_castle = true;
        bool prev_white_can_long_castle = true;
        bool prev_black_can_short_castle = true;
        bool prev_black_can_long_castle = true;
        bool prev_white_is_checked = false;
        bool prev_black_is_checked = false;

        PieceColor prev_turn = PieceColor::White;
        GameStatus prev_state = GameStatus::Normal;
        std::optional<PieceColor> prev_winner = std::nullopt;
        int prev_turn_counter = 1;
        int prev_last_pawn_or_capture = 1;
        bool prev_white_castled = false;
        bool prev_black_castled = false;
    };

    static bool make_move(Board &board, GameState &state, const BotMove &move, MoveRecord &record);

    static void undo_move(Board &board, GameState &state, const MoveRecord &record);

    static void update_game_state(Board &board, GameState &state, const MoveOutcome &outcome);

    static float minimax(Board &board, GameState &state, int depth, float alpha, float beta);
};


#endif //CHESSBOT_BOT_H