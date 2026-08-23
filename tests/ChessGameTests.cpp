#include <catch2/catch_test_macros.hpp>

#include "raylib.h"
#include "../src/ChessGame.h"

TEST_CASE("ChessGame starts with White's turn") {
    constexpr Texture2D testTexture{600, 200};
    ChessGame game(testTexture);
    game.state().state = GameStatus::Normal;
    
    REQUIRE(game.state().turn == PieceColor::White);
    REQUIRE(game.state().state == GameStatus::Normal);
}

TEST_CASE("ChessGame changes turn after a legal move") {
    constexpr Texture2D testTexture{600, 200};
    ChessGame game(testTexture);
    game.state().state = GameStatus::Normal;

    Piece* pawn = game.board().squares.at("E2").piece;
    BoardSquare& target = game.board().squares.at("E4");

    REQUIRE(pawn != nullptr);

    const auto result = game.try_move(*pawn, target);

    REQUIRE(result.has_value());
    REQUIRE(game.state().turn == PieceColor::Black);
    REQUIRE(pawn->square->name == "E4");
}

TEST_CASE("ChessGame rejects a move for the wrong colour") {
    constexpr Texture2D testTexture{600, 200};
    ChessGame game(testTexture);

    Piece* blackPawn = game.board().squares.at("E7").piece;
    BoardSquare& target = game.board().squares.at("E6");

    REQUIRE(blackPawn != nullptr);
    REQUIRE_FALSE(game.try_move(*blackPawn, target).has_value());
    REQUIRE(game.state().turn == PieceColor::White);
}