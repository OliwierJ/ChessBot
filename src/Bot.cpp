//
// Created by papie on 23/08/2026.
//

#include "Bot.h"

#include <iostream>
#include <limits>
#include <random>

#include "Board.h"
#include "Piece.h"
#include "BoardSquare.h"
#include "ChessGame.h"


std::vector<std::string> central_squares = {"C4", "C5", "D4", "D5", "E4", "E5", "F4", "F5"};

std::vector<BotMove> Bot::get_legal_moves(const PieceColor colour, const Board &board) {
    std::vector<BotMove> legal_moves;

    for (auto &piece: board.pieceList) {
        if (piece.colour != colour) continue;
        if (piece.legalMoves.empty()) continue;
        if (piece.captured) continue;

        for (const auto move: piece.legalMoves) {
            BotMove bot_move = {piece.square->name, move};
            legal_moves.push_back(bot_move);
        }
    }

    std::ranges::sort(legal_moves, [&](const BotMove& left, const BotMove& right) {
        const auto* left_piece = board.squares.at(left.target).piece;
        const auto* right_piece = board.squares.at(right.target).piece;

        const int left_value = left_piece == nullptr ? 0 : left_piece->value;
        const int right_value = right_piece == nullptr ? 0 : right_piece->value;

        return left_value > right_value;
    });
    return legal_moves;
}

float Bot::calculate_position(const Board &board, const GameState &state) {
    constexpr int checkmate_score = 100000;

    if (state.state == GameStatus::Checkmate) {
        return state.winner == PieceColor::White
                   ? checkmate_score
                   : -checkmate_score;
    }

    if (state.state == GameStatus::Stalemate) {
        return 0;
    }

    float score = 0;

    for (const Piece &piece: board.pieceList) {
        if (piece.captured) {
            continue;
        }

        const int value = piece.value;
        score += piece.colour == PieceColor::White ? value : -value;
        float square_control = 0;
        for (auto m: piece.attackingSquares) {
            if (std::ranges::count(central_squares, m)) {
                square_control += 0.018;
            } else {
                square_control += 0.002;
            }
        }
        if (std::ranges::count(central_squares, piece.square->name)) {
            square_control += 0.01;
        }

        if (piece.hasMoved && piece.type != PieceType::King && piece.type != PieceType::Queen) {
            score += piece.colour == PieceColor::White ? 0.01 : -0.01;
        }
        if (piece.hasMoved && piece.type == PieceType::King) {
            score -= piece.colour == PieceColor::White ? 1 : -1;
        }
        score += piece.colour == PieceColor::White ? square_control : -square_control;
    }

    if (state.black_castled) {
        score += -1;
    }
    if (state.white_castled) {
        score += 0.5;
    }

    return score;
}

std::optional<MoveOutcome> Bot::perform_bot_move(ChessGame &game) {
    const auto legal_moves = get_legal_moves(game.state().turn, game.board());
    if (legal_moves.empty()) {
        return std::nullopt;
    }

    constexpr float initial_score = std::numeric_limits<float>::max();
    float best_score = initial_score;
    std::vector<std::pair<BotMove, float> > best_moves;

    for (const auto &legal_move: legal_moves) {
        ChessGame simulated_game(game.board(), game.state());
        Piece *simulated_piece = simulated_game.board().squares.at(legal_move.from).piece;
        BoardSquare &simulated_target = simulated_game.board().squares.at(legal_move.target);

        if (!simulated_game.try_move(*simulated_piece, simulated_target)) {
            continue;
        }

        const auto white_simulates_moves = get_legal_moves(PieceColor::White, simulated_game.board());
        float best_white_response = std::numeric_limits<int>::min();

        for (const auto &[from, target]: white_simulates_moves) {
            ChessGame simulated_response(simulated_game.board(), simulated_game.state());
            Piece *response_piece = simulated_response.board().squares.at(from).piece;
            BoardSquare &response_target = simulated_response.board().squares.at(target);

            if (!simulated_response.try_move(*response_piece, response_target)) {
                continue;
            }

            const float response_eval =
                    calculate_position(simulated_response.board(), simulated_response.state());
            best_white_response = std::max(best_white_response, response_eval);

            // std::cout << "Eval: " << response_eval << " Best: " << best_white_response << " ";
        }

        // No response means the simulated position is already terminal.
        if (white_simulates_moves.empty()) {
            best_white_response = calculate_position(simulated_game.board(), simulated_game.state());
        }

        if (best_white_response < best_score) {
            best_score = best_white_response;
            best_moves.clear();
            best_moves.push_back({legal_move, best_score});
        } else if (best_white_response == best_score) {
            best_moves.push_back({legal_move, best_score});
        }
    }

    if (best_moves.empty()) {
        return std::nullopt;
    }

    std::cout << "\n";
    for (auto [fst, snd]: best_moves) {
        std::cout << fst.from << " " << fst.target << " " << snd << " ";
    }
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<size_t> dist(0, best_moves.size() - 1);

    const auto [from, target] = best_moves[dist(gen)].first;

    return game.try_move(*game.board().squares.at(from).piece, game.board().squares[target]);
}

std::optional<BotMove> Bot::choose_move(const Board &board, GameState &state, int depth, float alpha, float beta) {
    const auto legal_moves = get_legal_moves(state.turn, board);

    if (legal_moves.empty()) {
        return std::nullopt;
    }

    float best_score =
            state.turn == PieceColor::Black
                ? std::numeric_limits<float>::max()
                : std::numeric_limits<float>::lowest();

    std::vector<BotMove> best_moves;
    std::vector<std::pair<BotMove, float>> all_moves;

    for (const auto &move: legal_moves) {
        ChessGame simulated_game(board, state);

        Piece *piece = simulated_game.board().squares.at(move.from).piece;

        BoardSquare &target = simulated_game.board().squares.at(move.target);

        if (!simulated_game.try_move(*piece, target)) {
            continue;
        }

        const float score = minimax(simulated_game.board(), simulated_game.state(), depth - 1, alpha, beta);

        const bool better =
                state.turn == PieceColor::Black
                    ? score < best_score
                    : score > best_score;

        all_moves.emplace_back(move, score);

        if (better) {
            best_score = score;
            best_moves.clear();
            best_moves.push_back(move);
        } else if (score == best_score) {
            best_moves.push_back(move);
        }
    }

    if (best_moves.empty()) {
        return std::nullopt;
    }

    std::ranges::sort(all_moves,
        [](const std::pair<BotMove, float>& a, const std::pair<BotMove, float>& b) {
      return a.second < b.second;
    });

    for (auto m : all_moves) {
        std::cout << board.squares.at(m.first.from).piece->getPieceNotation() << m.first.target << " " << m.second << " ";
    }
    std::cout << "\n";

    static std::mt19937 generator(std::random_device{}());
    std::uniform_int_distribution<size_t> distribution(0, best_moves.size() - 1);

    return best_moves[distribution(generator)];
}

float Bot::minimax(const Board &board, const GameState &state, const int depth, float alpha, float beta) {
    if (depth == 0 ||
        state.state == GameStatus::Checkmate ||
        state.state == GameStatus::Stalemate) {
        return calculate_position(board, state);
    }

    const auto moves = get_legal_moves(state.turn, board);

    if (moves.empty()) {
        return calculate_position(board, state);
    }

    const bool maximizing = state.turn == PieceColor::White;

    if (maximizing) {
        float best_score = std::numeric_limits<float>::lowest();

        for (const auto& move : moves) {
            ChessGame simulated_game(board, state);
            Piece* piece =
                simulated_game.board().squares.at(move.from).piece;
            BoardSquare& target =
                simulated_game.board().squares.at(move.target);

            if (!simulated_game.try_move(*piece, target)) {
                continue;
            }

            const float score = minimax(
                simulated_game.board(),
                simulated_game.state(),
                depth - 1,
                alpha,
                beta
            );

            if (state.turn == PieceColor::Black) {
                best_score = std::min(best_score, score);
                beta = std::min(beta, best_score);
            } else {
                best_score = std::max(best_score, score);
                alpha = std::max(alpha, best_score);
            }

            if (beta <= alpha) {
                break;
            }
        }

        return best_score;
    }

    float best_score = std::numeric_limits<float>::max();

    for (const auto& move : moves) {
        ChessGame simulated_game(board, state);
        Piece* piece =
            simulated_game.board().squares.at(move.from).piece;
        BoardSquare& target =
            simulated_game.board().squares.at(move.target);

        if (!simulated_game.try_move(*piece, target)) {
            continue;
        }

        const float score = minimax(
            simulated_game.board(),
            simulated_game.state(),
            depth - 1,
            alpha,
            beta
        );

        best_score = std::min(best_score, score);
        beta = std::min(beta, best_score);

        if (beta <= alpha) {
            break;
        }
    }

    return best_score;
}
