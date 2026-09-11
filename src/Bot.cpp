#include "Bot.h"
#include <algorithm>
#include <limits>
#include <random>
#include "Board.h"
#include "BoardSquare.h"
#include "ChessGame.h"
#include "MoveValidator.h"
#include "Piece.h"

std::unordered_set<std::string> central_squares = {"C4", "C5", "D4", "D5", "E4", "E5", "F4", "F5"};
constexpr int checkmate_score = 100000;

std::vector<BotMove> Bot::get_legal_moves(const PieceColor colour, const Board &board) {
    std::vector<BotMove> legal_moves;

    for (auto &piece: board.pieceList) {
        if (piece.colour != colour) continue;
        if (piece.legalMoves.empty()) continue;
        if (piece.captured) continue;

        for (const auto& move: piece.legalMoves) {
            BotMove bot_move = {piece.square->name, move};
            legal_moves.push_back(bot_move);
        }
    }

    std::sort(legal_moves.begin(), legal_moves.end(), [&](const BotMove& left, const BotMove& right) {
        const auto* left_piece = board.squares.at(left.target).piece;
        const auto* right_piece = board.squares.at(right.target).piece;

        const int left_value = left_piece == nullptr ? 0 : left_piece->value;
        const int right_value = right_piece == nullptr ? 0 : right_piece->value;

        return left_value > right_value;
    });
    return legal_moves;
}

float Bot::calculate_position(const Board &board, const GameState &state) {
    if (state.state == GameStatus::Checkmate) {
        return state.winner == PieceColor::White
                   ? checkmate_score
                   : -checkmate_score;
    }

    if (state.state == GameStatus::Stalemate) {
        return 0;
    }

    float score = 0;

    // loop pieces
    for (const Piece &piece: board.pieceList) {
        if (piece.captured) {
            continue;
        }

        const int value = piece.value;
        score += piece.colour == PieceColor::White ? value : -value;
        float square_control = 0;

        // Loop attacking squares
        for (const auto& m: piece.attackingSquares) {
            if (central_squares.contains(m)) {
                square_control += 0.018;
            } else {
                square_control += 0.002;
            }

            // give points for attacking enemy pieces
            if (board.square_contains_opponent_piece(m, piece.colour)) {
                square_control += 0.02;
            }
        }

        // give points for controlling central squares
        if (central_squares.contains(piece.square->name)) {
            square_control += 0.01;
        }

        // give points for moving unmoved pieces
        if (piece.hasMoved && piece.type != PieceType::King && piece.type != PieceType::Queen) {
            score += piece.colour == PieceColor::White ? 0.01 : -0.01;
        }

        // take away points for moving king
        if (piece.hasMoved && piece.type == PieceType::King) {
            score -= piece.colour == PieceColor::White ? 1 : -1;
        }

        score += piece.colour == PieceColor::White ? square_control : -square_control;
    }

    if (state.black_castled) {
        score += -2;
    }
    if (state.white_castled) {
        score += 2;
    }

    return score;
}

bool Bot::make_move(Board &board, GameState &state, const BotMove &move, MoveRecord &record) {
    record.prev_turn = state.turn;
    record.prev_state = state.state;
    record.prev_winner = state.winner;
    record.prev_turn_counter = state.turn_counter;
    record.prev_last_pawn_or_capture = state.last_pawn_or_capture;
    record.prev_white_castled = state.white_castled;
    record.prev_black_castled = state.black_castled;

    record.prev_white_can_short_castle = board.whiteCanShortCastle;
    record.prev_white_can_long_castle = board.whiteCanLongCastle;
    record.prev_black_can_short_castle = board.blackCanShortCastle;
    record.prev_black_can_long_castle = board.blackCanLongCastle;
    record.prev_white_is_checked = board.whiteIsChecked;
    record.prev_black_is_checked = board.blackIsChecked;
    record.prev_en_passant_squares = board.enpassantSquares;

    record.from_square = &board.squares.at(move.from);
    record.to_square = &board.squares.at(move.target);
    record.moved_piece = record.from_square->piece;

    if (record.moved_piece == nullptr || record.moved_piece->colour != state.turn) {
        return false;
    }

    if (!record.moved_piece->isLegalMove(move.target)) {
        return false;
    }

    record.moved_piece_prev_has_moved = record.moved_piece->hasMoved;
    record.moved_piece_prev_type = record.moved_piece->type;
    record.moved_piece_prev_value = record.moved_piece->value;
    record.moved_piece_prev_texture = record.moved_piece->pieceTexture;
    record.moved_piece_prev_pos = record.moved_piece->getCurrentPos();
    record.moved_piece_prev_last_pos = record.moved_piece->lastPosition;

    if (record.moved_piece->type == PieceType::King) {
        if (state.turn == PieceColor::White && move.target == "G1") {
            record.rook_from_square = &board.squares.at("H1");
            record.rook_to_square = &board.squares.at("F1");
        } else if (state.turn == PieceColor::White && move.target == "C1") {
            record.rook_from_square = &board.squares.at("A1");
            record.rook_to_square = &board.squares.at("D1");
        } else if (state.turn == PieceColor::Black && move.target == "G8") {
            record.rook_from_square = &board.squares.at("H8");
            record.rook_to_square = &board.squares.at("F8");
        } else if (state.turn == PieceColor::Black && move.target == "C8") {
            record.rook_from_square = &board.squares.at("A8");
            record.rook_to_square = &board.squares.at("D8");
        }

        if (record.rook_from_square != nullptr) {
            record.rook_piece = record.rook_from_square->piece;
            if (record.rook_piece != nullptr) {
                record.rook_prev_has_moved = record.rook_piece->hasMoved;
                record.rook_prev_pos = record.rook_piece->getCurrentPos();
                record.rook_prev_last_pos = record.rook_piece->lastPosition;
            }
        }
    }

    if (board.enpassantSquares.contains(move.target)) {
        const int up_or_down = state.turn == PieceColor::White ? -1 : 1;
        const std::string captured_square_name = {
            move.target[0],
            static_cast<char>(move.target[1] + up_or_down)
        };
        record.captured_piece_square = &board.squares.at(captured_square_name);
        record.captured_piece = record.captured_piece_square->piece;
        record.was_en_passant_capture = record.captured_piece != nullptr;
    } else {
        record.captured_piece = record.to_square->piece;
        record.captured_piece_square = record.to_square;
    }

    if (record.captured_piece != nullptr) {
        record.captured_piece_prev_captured = record.captured_piece->captured;
    }

    const MoveOutcome outcome = MoveValidator::apply_move(*record.moved_piece, board, *record.to_square);
    update_game_state(board, state, outcome);
    return true;
}

void Bot::undo_move(Board &board, GameState &state, const MoveRecord &record) {
    board.whiteCanShortCastle = record.prev_white_can_short_castle;
    board.whiteCanLongCastle = record.prev_white_can_long_castle;
    board.blackCanShortCastle = record.prev_black_can_short_castle;
    board.blackCanLongCastle = record.prev_black_can_long_castle;
    board.whiteIsChecked = record.prev_white_is_checked;
    board.blackIsChecked = record.prev_black_is_checked;
    board.enpassantSquares = record.prev_en_passant_squares;

    if (record.rook_piece != nullptr && record.rook_from_square != nullptr && record.rook_to_square != nullptr) {
        record.rook_to_square->piece = nullptr;
        record.rook_from_square->piece = record.rook_piece;
        record.rook_piece->square = record.rook_from_square;
        record.rook_piece->hasMoved = record.rook_prev_has_moved;
        record.rook_piece->setCurrentPos(record.rook_prev_pos);
        record.rook_piece->lastPosition = record.rook_prev_last_pos;
    }

    if (record.to_square->piece == record.moved_piece) {
        record.to_square->piece = nullptr;
    }

    if (record.captured_piece != nullptr) {
        record.captured_piece->captured = record.captured_piece_prev_captured;
        record.captured_piece->square = record.captured_piece_square;
        record.captured_piece->setCurrentPos(
            {record.captured_piece_square->squareBox.x, record.captured_piece_square->squareBox.y}
        );
        record.captured_piece_square->piece = record.captured_piece;
    } else if (!record.was_en_passant_capture) {
        record.to_square->piece = nullptr;
    }

    record.from_square->piece = record.moved_piece;
    record.moved_piece->square = record.from_square;
    record.moved_piece->hasMoved = record.moved_piece_prev_has_moved;
    record.moved_piece->type = record.moved_piece_prev_type;
    record.moved_piece->value = record.moved_piece_prev_value;
    record.moved_piece->pieceTexture = record.moved_piece_prev_texture;
    record.moved_piece->setCurrentPos(record.moved_piece_prev_pos);
    record.moved_piece->lastPosition = record.moved_piece_prev_last_pos;

    state.turn = record.prev_turn;
    state.state = record.prev_state;
    state.winner = record.prev_winner;
    state.turn_counter = record.prev_turn_counter;
    state.last_pawn_or_capture = record.prev_last_pawn_or_capture;
    state.white_castled = record.prev_white_castled;
    state.black_castled = record.prev_black_castled;
}

void Bot::update_game_state(Board &board, GameState &state, const MoveOutcome &outcome) {
    if (state.turn == PieceColor::Black) {
        state.turn_counter++;
    }

    if (state.turn_counter - state.last_pawn_or_capture >= 50) {
        state.state = GameStatus::Stalemate;
        state.winner = std::nullopt;
        return;
    }

    if (outcome.pieceTaken || outcome.pawnMoved) {
        state.last_pawn_or_capture = state.turn_counter;
    }

    if (outcome.shortCastled || outcome.longCastled) {
        if (state.turn == PieceColor::White) {
            state.white_castled = true;
        } else {
            state.black_castled = true;
        }
    }

    const PieceColor opponent = opposite(state.turn);
    const bool checked = board.isColourChecked(opponent);
    const bool has_legal_moves = board.getLegalMoveCount(opponent) > 0;

    if (checked && !has_legal_moves) {
        state.state = GameStatus::Checkmate;
        state.winner = state.turn;
    } else if (!checked && !has_legal_moves) {
        state.state = GameStatus::Stalemate;
        state.winner = std::nullopt;
    } else {
        state.state = GameStatus::Normal;
        state.winner = std::nullopt;
    }

    if (state.state == GameStatus::Normal) {
        state.turn = opposite(state.turn);
    }
}

std::optional<BotMove> Bot::choose_move(const Board &board, const GameState &state, const int depth, float alpha, float beta) {
    Board search_board = board;
    GameState search_state = state;

    const auto legal_moves = get_legal_moves(search_state.turn, search_board);
    if (legal_moves.empty()) {
        return std::nullopt;
    }

    const bool maximizing = search_state.turn == PieceColor::White;
    float best_score = maximizing
                           ? std::numeric_limits<float>::lowest()
                           : std::numeric_limits<float>::max();
    std::vector<BotMove> best_moves;

    for (const auto &move: legal_moves) {
        MoveRecord record;
        if (!make_move(search_board, search_state, move, record)) {
            continue;
        }

        const float score = minimax(search_board, search_state, depth - 1, alpha, beta);
        undo_move(search_board, search_state, record);

        const bool better = maximizing ? score > best_score : score < best_score;
        if (better) {
            best_score = score;
            best_moves.clear();
            best_moves.push_back(move);
        } else if (score == best_score) {
            best_moves.push_back(move);
        }

        if (maximizing) {
            alpha = std::max(alpha, best_score);
        } else {
            beta = std::min(beta, best_score);
        }

        if (beta <= alpha) {
            break;
        }
    }

    if (best_moves.empty()) {
        return std::nullopt;
    }

    static std::mt19937 generator(std::random_device{}());
    std::uniform_int_distribution<size_t> distribution(0, best_moves.size() - 1);
    return best_moves[distribution(generator)];
}

float Bot::minimax(Board &board, GameState &state, const int depth, float alpha, float beta) {
    if (depth == 0 || state.state == GameStatus::Checkmate || state.state == GameStatus::Stalemate) {
        return calculate_position(board, state);
    }

    const auto moves = get_legal_moves(state.turn, board);
    if (moves.empty()) {
        return calculate_position(board, state);
    }

    const bool maximizing = state.turn == PieceColor::White;
    float best_score = maximizing
                           ? std::numeric_limits<float>::lowest()
                           : std::numeric_limits<float>::max();
    bool evaluated_any_move = false;

    for (const auto &move: moves) {
        MoveRecord record;
        if (!make_move(board, state, move, record)) {
            continue;
        }

        evaluated_any_move = true;
        const float score = minimax(board, state, depth - 1, alpha, beta);
        undo_move(board, state, record);

        if (maximizing) {
            best_score = std::max(best_score, score);
            alpha = std::max(alpha, best_score);
        } else {
            best_score = std::min(best_score, score);
            beta = std::min(beta, best_score);
        }

        if (beta <= alpha) {
            break;
        }
    }

    if (!evaluated_any_move) {
        return calculate_position(board, state);
    }

    return best_score;
}
