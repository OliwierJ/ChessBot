//
// Created by papie on 22/08/2026.
//

#include <thread>
#include "ChessGame.h"
#include "Board.h"
#include "GameState.h"
#include "MoveValidator.h"


ChessGame::ChessGame(const Texture2D &piecesTexture) {
    board().set_up_pieces(piecesTexture);
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

// std::optional<MoveOutcome> ChessGame::perform_bot_move() {
//     bool found_piece = false;
//     Piece *random_piece = nullptr;
//     std::string move;
//
//     // Bot behaviour
//     while (!found_piece) {
//         const int randomPieceIndex = std::rand() % gameBoard.pieceList.size();
//         random_piece = &gameBoard.pieceList[randomPieceIndex];
//         if (random_piece->captured) continue;
//         if (random_piece->colour != gameState.turn) continue;
//         if (random_piece->legalMoves.empty()) continue;
//
//         const int randomMoveIndex = std::rand() % random_piece->legalMoves.size();
//         move = random_piece->legalMoves[randomMoveIndex];
//         found_piece = true;
//     }
//
//     std::this_thread::sleep_for(std::chrono::milliseconds(100));
//     return try_move(*random_piece, gameBoard.squares[move]);
// }

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
}

void ChessGame::complete_move(const Piece &piece, const BoardSquare &target, const std::string &previousPosition,
                              const MoveOutcome outcome) {
    const std::string notation = create_notation(piece, target, previousPosition, outcome);

    gameState.move_history.append_move(notation);

    // update turn counts and game end flags
    update_game_status(outcome);

    // swap turn colour
    if (gameState.state == GameStatus::Normal) {
        gameState.turn = opposite(gameState.turn);
    }

}

void ChessGame::update_game_status(const MoveOutcome outcome) {

    // increment turn count on blacks move
    if (gameState.turn == PieceColor::Black) gameState.turn_counter++;

    // check if 50 moves since a pawn move or capture
    if (gameState.turn_counter - gameState.last_pawn_or_capture >= 50) {
        gameState.state = GameStatus::Stalemate; return;
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

    if (gameBoard.isColourChecked(opposite(gameState.turn)) && gameState.state != GameStatus::Checkmate) move_notation += '+';
    if (gameState.state == GameStatus::Checkmate) move_notation += '#';

    return move_notation;
}
