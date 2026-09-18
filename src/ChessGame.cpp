#include "ChessGame.h"
#include <iostream>
#include <random>
#include "Board.h"
#include "GameState.h"
#include "MoveValidator.h"
#include "Zobrist.h"


ChessGame::ChessGame(const Texture2D &piecesTexture) {
    board().set_up_pieces(piecesTexture);
    std::mt19937_64 generator(0x1234567);
    std::uniform_int_distribution<uint64_t> distribution;
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 6; j++) {
            for (int k = 0; k < 64; k++) {
                Zobrist::pieces_hash[i][j][k] = distribution(generator);
            }
        }
    }
    for (int i = 0; i < 4; i++) {
        Zobrist::castling_hash[i] = distribution(generator);
    }
    for (int i = 0; i < 8; i++) {
        Zobrist::enpassant_file_hash[i] = distribution(generator);
    }
    Zobrist::side_to_move_hash = distribution(generator);

    gameState.position_table[gameBoard.generate_hash(gameState)]++;
}

ChessGame::ChessGame(const Board& board, const GameState& state) : gameBoard(board), gameState(state) {}

std::optional<MoveOutcome> ChessGame::try_move(Piece &piece, BoardSquare &target) {
    if (gameState.state != GameStatus::Normal) {
        return std::nullopt;
    }

    if (piece.colour != gameState.turn) {
        return std::nullopt;
    }

    if (!MoveValidator::validate_legal_move(piece, target, gameBoard)) {
        return std::nullopt;
    }

    const std::string previousPosition = piece.square->name;
    MoveOutcome outcome = MoveValidator::apply_move(piece, gameBoard, target);

    complete_move(piece, target, previousPosition, outcome);

    return outcome;
}

const Board &ChessGame::board() const {
    return gameBoard;
}

Board &ChessGame::board() {
    return gameBoard;
}

GameState &ChessGame::state() {
    return gameState;
}

void ChessGame::restart_game(const Texture2D &pieceTexture) {
    gameBoard.clear_board();
    gameBoard.set_up_pieces(pieceTexture);
    gameState.reset_state();
    gameState.position_table[gameBoard.generate_hash(gameState)]++;
}

void ChessGame::complete_move(const Piece &piece, const BoardSquare &target, const std::string &previousPosition,
                              const MoveOutcome outcome) {
    const std::string notation = create_notation(piece, target, previousPosition, outcome);


    // update turn counts and game end flags
    update_game_status(outcome);

    gameState.move_history.append_move(notation);

    // swap turn colour
    if (gameState.state == GameStatus::Normal) {
        gameState.turn = opposite(gameState.turn);
    }

}

bool ChessGame::check_draw_by_insufficient_material() const {
    // check draw by insufficient material
    int white_piece_count = 0;
    int white_knights_count = 0;
    int white_bishop_count = 0;
    int black_piece_count = 0;
    int black_knights_count = 0;
    int black_bishop_count = 0;
    for (const auto& p : gameBoard.pieceList) {
        if (p.captured) continue;
        if (p.colour == PieceColor::White) {
            if (p.type == PieceType::Knight) white_knights_count++;
            if (p.type == PieceType::Bishop) white_bishop_count++;
            white_piece_count++;
        }
        else {
            if (p.type == PieceType::Knight) black_knights_count++;
            if (p.type == PieceType::Bishop) black_bishop_count++;
            black_piece_count++;
        }
    }
    // king v king
    if (white_piece_count == 1 && black_piece_count == 1) {
        return true;
    }
    // b king v knight/bishop + king
    if (black_piece_count == 1 && white_piece_count == 2 && (white_bishop_count == 1 || white_knights_count == 1)) {
        return true;
    }

    // b king + knight/bishop v knight/bishop + king
    if (black_piece_count == 2 && (black_bishop_count == 1 || black_knights_count == 1) && white_piece_count == 2 && (white_bishop_count == 1 || white_knights_count == 1)) {
        return true;
    }

    // b king v 2 knight + king
    if (black_piece_count == 1 && white_piece_count == 3 && white_knights_count == 2) {
        return true;
    }

    // w king v knight/bishop + king
    if (white_piece_count == 1 && black_piece_count == 2 && (black_bishop_count == 1 || black_knights_count == 1)) {
        return true;
    }

    // w king v 2 knight + king
    if (white_piece_count == 1 && black_piece_count == 3 && black_knights_count == 2) {
        return true;
    }

    return false;
}

void ChessGame::update_game_status(const MoveOutcome outcome) {

    // increment turn count on blacks move
    if (gameState.turn == PieceColor::Black) gameState.turn_counter++;

    // check if 50 moves since a pawn move or capture
    if (gameState.turn_counter - gameState.last_pawn_or_capture >= 50) {
        gameState.state = GameStatus::Stalemate; return;
    }

    if (check_draw_by_insufficient_material()) {
        gameState.state = GameStatus::Draw;
        return;
    }

    const auto new_hash = gameBoard.generate_hash(gameState);
    gameState.position_table[new_hash]++;
    if (gameState.position_table[new_hash] == 3) {
        gameState.state = GameStatus::Draw;
        return;
    }

    if (outcome.pieceTaken || outcome.pawnMoved) gameState.last_pawn_or_capture = gameState.turn_counter;

    if (outcome.shortCastled || outcome.longCastled) {
        if (gameState.turn == PieceColor::White) {
            gameState.white_castled = true;
        } else {
            gameState.black_castled = true;
        }
    }
    const PieceColor opponent = opposite(gameState.turn);
    const bool checked = gameBoard.isColourChecked(opponent);
    const bool hasLegalMoves =
        gameBoard.getLegalMoveCount(opponent) > 0;

    if (checked && !hasLegalMoves) {
        gameState.state = GameStatus::Checkmate;
        gameState.winner = gameState.turn;
    } else if (!checked && !hasLegalMoves) {
        gameState.state = GameStatus::Stalemate;
    }
}

std::string ChessGame::create_notation(const Piece &piece, const BoardSquare &target,
                                       const std::string &previousPosition, const MoveOutcome &outcome) const {
    // calculate move notation
    std::string move_notation = {static_cast<char>(target.name[0] + 32), target.name[1]};

    if (outcome.pieceTaken && piece.type != PieceType::Pawn) {
        move_notation = {piece.getPieceNotation(), 'x', move_notation[0], move_notation[1]};
    } else if (outcome.pieceTaken) {
        move_notation = {static_cast<char>(previousPosition[0] + 32), 'x', move_notation[0], move_notation[1]};
    } else if (outcome.shortCastled) {
        move_notation = "O-O";
    } else if (outcome.longCastled) {
        move_notation = "O-O-O";
    } else {
        move_notation = {piece.getPieceNotation(), move_notation[0], move_notation[1]};
    }
    if (outcome.pawnPromoted) move_notation += "=Q";
    if (gameBoard.isColourChecked(opposite(gameState.turn)) && gameState.state != GameStatus::Checkmate) move_notation += '+';
    if (gameState.state == GameStatus::Checkmate) move_notation += '#';

    return move_notation;
}
