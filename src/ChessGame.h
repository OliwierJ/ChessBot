//
// Created by papie on 22/08/2026.
//

#ifndef CHESSBOT_CHESSGAME_H
#define CHESSBOT_CHESSGAME_H
#include <optional>
#include <string>

#include "Board.h"
#include "GameState.h"
#include "MoveValidator.h"
#include "raylib.h"

class GameState;
class Board;
class Piece;
class BoardSquare;

class ChessGame {
public:
    explicit ChessGame(const Texture2D& piecesTexture);

    std::optional<MoveOutcome> try_move(Piece &piece, BoardSquare &target);

    std::optional<MoveOutcome> perform_bot_move();

    [[nodiscard]]
    const Board &board() const;

    Board &board();

    GameState &state();

    void restart_game(const Texture2D &pieceTexture);

private:
    Board gameBoard;
    GameState gameState;

    void complete_move(
        const Piece &piece,
        const BoardSquare &target,
        const std::string &previousPosition,
        MoveOutcome outcome
    );

    void update_game_status(MoveOutcome outcome);

    [[nodiscard]]
    std::string create_notation(
        const Piece &piece,
        const BoardSquare &target,
        const std::string &previousPosition,
        const MoveOutcome &outcome
    ) const;
};


#endif //CHESSBOT_CHESSGAME_H
