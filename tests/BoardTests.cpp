#include <catch2/catch_test_macros.hpp>

#include "../Board.h"
#include "../Piece.h"
#include "../MoveValidator.h"

TEST_CASE("A new board contains all starting pieces") {
    Texture2D testTexture{600, 200};
    Board board;
    board.set_up_pieces(testTexture);

    REQUIRE(board.pieceList.size() == 32);
    // white
    {
        REQUIRE(board.squares.at("A1").piece->type == PieceType::Rook);
        REQUIRE(board.squares.at("A1").piece->colour == PieceColor::White);
        REQUIRE(board.squares.at("B1").piece->type == PieceType::Knight);
        REQUIRE(board.squares.at("B1").piece->colour == PieceColor::White);
        REQUIRE(board.squares.at("C1").piece->type == PieceType::Bishop);
        REQUIRE(board.squares.at("C1").piece->colour == PieceColor::White);
        REQUIRE(board.squares.at("D1").piece->type == PieceType::Queen);
        REQUIRE(board.squares.at("D1").piece->colour == PieceColor::White);
        REQUIRE(board.squares.at("E1").piece->type == PieceType::King);
        REQUIRE(board.squares.at("E1").piece->colour == PieceColor::White);
        REQUIRE(board.squares.at("F1").piece->type == PieceType::Bishop);
        REQUIRE(board.squares.at("F1").piece->colour == PieceColor::White);
        REQUIRE(board.squares.at("G1").piece->type == PieceType::Knight);
        REQUIRE(board.squares.at("G1").piece->colour == PieceColor::White);
        REQUIRE(board.squares.at("H1").piece->type == PieceType::Rook);
        REQUIRE(board.squares.at("H1").piece->colour == PieceColor::White);

        REQUIRE(board.squares.at("A2").piece->type == PieceType::Pawn);
        REQUIRE(board.squares.at("A2").piece->colour == PieceColor::White);
        REQUIRE(board.squares.at("B2").piece->type == PieceType::Pawn);
        REQUIRE(board.squares.at("B2").piece->colour == PieceColor::White);
        REQUIRE(board.squares.at("C2").piece->type == PieceType::Pawn);
        REQUIRE(board.squares.at("C2").piece->colour == PieceColor::White);
        REQUIRE(board.squares.at("D2").piece->type == PieceType::Pawn);
        REQUIRE(board.squares.at("D2").piece->colour == PieceColor::White);
        REQUIRE(board.squares.at("E2").piece->type == PieceType::Pawn);
        REQUIRE(board.squares.at("E2").piece->colour == PieceColor::White);
        REQUIRE(board.squares.at("F2").piece->type == PieceType::Pawn);
        REQUIRE(board.squares.at("F2").piece->colour == PieceColor::White);
        REQUIRE(board.squares.at("G2").piece->type == PieceType::Pawn);
        REQUIRE(board.squares.at("G2").piece->colour == PieceColor::White);
        REQUIRE(board.squares.at("H2").piece->type == PieceType::Pawn);
        REQUIRE(board.squares.at("H2").piece->colour == PieceColor::White);

        REQUIRE(board.squares.at("E8").piece != nullptr);
        REQUIRE(board.squares.at("E8").piece->type == PieceType::King);
        REQUIRE(board.squares.at("E8").piece->colour == PieceColor::Black);
    }
    // black
    {
        REQUIRE(board.squares.at("A8").piece->type == PieceType::Rook);
        REQUIRE(board.squares.at("A8").piece->colour == PieceColor::Black);
        REQUIRE(board.squares.at("B8").piece->type == PieceType::Knight);
        REQUIRE(board.squares.at("B8").piece->colour == PieceColor::Black);
        REQUIRE(board.squares.at("C8").piece->type == PieceType::Bishop);
        REQUIRE(board.squares.at("C8").piece->colour == PieceColor::Black);
        REQUIRE(board.squares.at("D8").piece->type == PieceType::Queen);
        REQUIRE(board.squares.at("D8").piece->colour == PieceColor::Black);
        REQUIRE(board.squares.at("E8").piece->type == PieceType::King);
        REQUIRE(board.squares.at("E8").piece->colour == PieceColor::Black);
        REQUIRE(board.squares.at("F8").piece->type == PieceType::Bishop);
        REQUIRE(board.squares.at("F8").piece->colour == PieceColor::Black);
        REQUIRE(board.squares.at("G8").piece->type == PieceType::Knight);
        REQUIRE(board.squares.at("G8").piece->colour == PieceColor::Black);
        REQUIRE(board.squares.at("H8").piece->type == PieceType::Rook);
        REQUIRE(board.squares.at("H8").piece->colour == PieceColor::Black);

        REQUIRE(board.squares.at("A7").piece->type == PieceType::Pawn);
        REQUIRE(board.squares.at("A7").piece->colour == PieceColor::Black);
        REQUIRE(board.squares.at("B7").piece->type == PieceType::Pawn);
        REQUIRE(board.squares.at("B7").piece->colour == PieceColor::Black);
        REQUIRE(board.squares.at("C7").piece->type == PieceType::Pawn);
        REQUIRE(board.squares.at("C7").piece->colour == PieceColor::Black);
        REQUIRE(board.squares.at("D7").piece->type == PieceType::Pawn);
        REQUIRE(board.squares.at("D7").piece->colour == PieceColor::Black);
        REQUIRE(board.squares.at("E7").piece->type == PieceType::Pawn);
        REQUIRE(board.squares.at("E7").piece->colour == PieceColor::Black);
        REQUIRE(board.squares.at("F7").piece->type == PieceType::Pawn);
        REQUIRE(board.squares.at("F7").piece->colour == PieceColor::Black);
        REQUIRE(board.squares.at("G7").piece->type == PieceType::Pawn);
        REQUIRE(board.squares.at("G7").piece->colour == PieceColor::Black);
        REQUIRE(board.squares.at("H7").piece->type == PieceType::Pawn);
        REQUIRE(board.squares.at("H7").piece->colour == PieceColor::Black);
    }

    // king pointers
    REQUIRE(board.whiteKing->square->name == "E1");
    REQUIRE(board.blackKing->square->name == "E8");
}

TEST_CASE("White pawn has one and two square opening moves") {
    Texture2D testTexture{600, 200};
    Board board;
    board.set_up_pieces(testTexture);

    Piece *pawn = board.squares.at("E2").piece;

    REQUIRE(pawn != nullptr);
    REQUIRE(pawn->type == PieceType::Pawn);
    REQUIRE(pawn->hasMoved == false);
    REQUIRE(pawn->isLegalMove("E3"));
    REQUIRE(pawn->isLegalMove("E4"));
}

TEST_CASE("Knight has 8 possible moves") {
    constexpr Texture2D testTexture{600, 200};
    Board board;
    board.addPieceToBoard(PieceType::Knight, PieceColor::White, "E4", testTexture);

    Piece *knight = board.squares.at("E4").piece;
    knight->calculateLegalMoves(&board);

    REQUIRE(knight != nullptr);
    REQUIRE(knight->type == PieceType::Knight);
    REQUIRE(knight->isLegalMove("C5"));
    REQUIRE(knight->isLegalMove("C3"));
    REQUIRE(knight->isLegalMove("D6"));
    REQUIRE(knight->isLegalMove("D2"));
    REQUIRE(knight->isLegalMove("F6"));
    REQUIRE(knight->isLegalMove("F2"));
    REQUIRE(knight->isLegalMove("G5"));
    REQUIRE(knight->isLegalMove("G3"));
}

TEST_CASE("Cannot move piece into check") {
    constexpr Texture2D testTexture{600, 200};
    Board board;
    board.addPieceToBoard(PieceType::Rook, PieceColor::White, "E4", testTexture);
    board.addPieceToBoard(PieceType::King, PieceColor::White, "E3", testTexture);
    board.whiteKing = &board.pieceList.back();
    board.addPieceToBoard(PieceType::Rook, PieceColor::Black, "E8", testTexture);
    board.addPieceToBoard(PieceType::King, PieceColor::Black, "A8", testTexture);
    board.blackKing = &board.pieceList.back();

    board.calculateAllLegalMoves();
    board.calculateAllLegalMovesByColour(PieceColor::Black);
    board.calculateAllLegalMovesByColour(PieceColor::White);
    Piece *rook = board.squares.at("E4").piece;

    REQUIRE(!rook->isLegalMove("D4"));
    REQUIRE(!rook->isLegalMove("C4"));
}

TEST_CASE("Black pawn has one and two square opening moves") {
    constexpr Texture2D testTexture{600, 200};
    Board board;
    board.set_up_pieces(testTexture);

    Piece *pawn = board.squares.at("E7").piece;

    REQUIRE(pawn != nullptr);
    REQUIRE(pawn->type == PieceType::Pawn);
    REQUIRE(pawn->isLegalMove("E6"));
    REQUIRE(pawn->isLegalMove("E5"));
}

TEST_CASE("Knight cannot move onto a friendly piece") {
    constexpr Texture2D testTexture{600, 200};
    Board board;
    board.addBothKings();

    board.addPieceToBoard(PieceType::Knight, PieceColor::White, "E4", testTexture);

    board.addPieceToBoard(PieceType::Pawn, PieceColor::White, "C5", testTexture);

    Piece *knight = board.squares.at("E4").piece;
    knight->calculateLegalMoves(&board);

    REQUIRE_FALSE(knight->isLegalMove("C5"));
    REQUIRE(knight->isLegalMove("C3"));
}

TEST_CASE("Rook moves along an empty rank") {
    constexpr Texture2D testTexture{600, 200};
    Board board;
    board.addBothKings();

    board.addPieceToBoard(PieceType::Rook, PieceColor::White, "E4", testTexture);

    Piece *rook = board.squares.at("E4").piece;
    rook->calculateLegalMoves(&board);

    REQUIRE(rook->isLegalMove("A4"));
    REQUIRE(rook->isLegalMove("B4"));
    REQUIRE(rook->isLegalMove("H4"));
    REQUIRE(rook->isLegalMove("E1"));
    REQUIRE(rook->isLegalMove("E8"));
}

TEST_CASE("Rook cannot move through a piece") {
    constexpr Texture2D testTexture{600, 200};
    Board board;
    board.addBothKings();

    board.addPieceToBoard(PieceType::Rook, PieceColor::White, "A1", testTexture);

    board.addPieceToBoard(PieceType::Pawn, PieceColor::White, "D1", testTexture);

    Piece *rook = board.squares.at("A1").piece;
    rook->calculateLegalMoves(&board);

    REQUIRE(rook->isLegalMove("B1"));
    REQUIRE(rook->isLegalMove("C1"));
    // REQUIRE_FALSE(rook->isLegalMove("D1"));
    REQUIRE_FALSE(rook->isLegalMove("E1"));
}

TEST_CASE("A piece can capture an opposing piece") {
    constexpr Texture2D testTexture{600, 200};
    Board board;
    board.addBothKings();

    board.addPieceToBoard(PieceType::Rook, PieceColor::White, "A1", testTexture);

    board.addPieceToBoard(PieceType::Pawn, PieceColor::Black, "A4", testTexture);

    Piece *rook = board.squares.at("A1").piece;
    BoardSquare &target = board.squares.at("A4");

    rook->calculateLegalMoves(&board);

    REQUIRE(rook->isLegalMove("A4"));
    REQUIRE(MoveValidator::validate_legal_move(*rook, target, board));

    const MoveOutcome result =
            MoveValidator::apply_move(*rook, board, target);

    REQUIRE(result.validMove);
    REQUIRE(result.pieceTaken);
    REQUIRE(target.piece == rook);
    REQUIRE(rook->square == &target);
}

TEST_CASE("A piece cannot capture a friendly piece") {
    constexpr Texture2D testTexture{600, 200};
    Board board;

    board.addBothKings();

    board.addPieceToBoard(PieceType::Rook, PieceColor::White, "A1", testTexture);

    board.addPieceToBoard(PieceType::Pawn, PieceColor::White, "A4", testTexture);

    Piece *rook = board.squares.at("A1").piece;
    BoardSquare &target = board.squares.at("A4");

    rook->calculateLegalMoves(&board);
    REQUIRE_FALSE(MoveValidator::validate_legal_move(*rook, target, board));
}

TEST_CASE("Applying a move updates both board and piece position") {
    constexpr Texture2D testTexture{600, 200};
    Board board;

    board.addBothKings();
    board.addPieceToBoard(PieceType::Rook, PieceColor::White, "A1", testTexture);

    Piece *rook = board.squares.at("A1").piece;
    BoardSquare &target = board.squares.at("A4");

    rook->calculateLegalMoves(&board);
    const MoveOutcome result =
            MoveValidator::apply_move(*rook, board, target);

    REQUIRE(rook != nullptr);
    REQUIRE(result.validMove);
    REQUIRE(board.squares.at("A1").piece == nullptr);
    REQUIRE(board.squares.at("A4").piece == rook);
    REQUIRE(rook->square == &target);
}

TEST_CASE("Promoting a pawn") {
    constexpr Texture2D testTexture{600, 200};
    Board board;

    board.addBothKings();
    board.addPieceToBoard(PieceType::Pawn, PieceColor::White, "A7", testTexture);

    Piece *pawn = board.squares.at("A7").piece;
    BoardSquare &target = board.squares.at("A8");

    pawn->calculateLegalMoves(&board);
    const MoveOutcome result =
            MoveValidator::apply_move(*pawn, board, target);
    
    REQUIRE(pawn != nullptr);
    REQUIRE(result.validMove);
    REQUIRE(pawn->type == PieceType::Queen);
    REQUIRE(pawn->square->name == "A8");
}

TEST_CASE("Pawn taking en passant") {
    constexpr Texture2D testTexture{600, 200};
    Board board;

    board.addBothKings();
    board.addPieceToBoard(PieceType::Pawn, PieceColor::White, "A2", testTexture);
    board.addPieceToBoard(PieceType::Pawn, PieceColor::Black, "B4", testTexture);

    Piece *white_pawn = board.squares.at("A2").piece;
    Piece *black_pawn = board.squares.at("B4").piece;
    BoardSquare &target = board.squares.at("A4");
    BoardSquare &enpassant = board.squares.at("A3");

    REQUIRE(white_pawn != nullptr);
    REQUIRE(black_pawn != nullptr);

    white_pawn->calculateLegalMoves(&board);
    MoveValidator::apply_move(*white_pawn, board, target);
    REQUIRE(std::ranges::count(board.enpassantSquares, "A3") == 1);
    REQUIRE(black_pawn->isLegalMove("A3"));

    const MoveOutcome result = MoveValidator::apply_move(*black_pawn, board, enpassant);
    REQUIRE(board.enpassantSquares.empty());
    REQUIRE(black_pawn->square->name == "A3");
    REQUIRE(result.pieceTaken);
}

TEST_CASE("White checks black king") {
    constexpr Texture2D testTexture{600, 200};
    Board board;

    board.addBothKings();
    board.addPieceToBoard(PieceType::Queen, PieceColor::White, "A1", testTexture);

    Piece* queen = board.squares.at("A1").piece;
    Piece* king = board.squares.at("E8").piece;
    BoardSquare &target = board.squares.at("A8");

    REQUIRE(queen != nullptr);

    MoveOutcome result = MoveValidator::apply_move(*queen, board, target);
    REQUIRE(result.check);
    REQUIRE(queen->square->name == "A8");
    REQUIRE(board.blackIsChecked);
    REQUIRE(!king->isLegalMove("D8"));
}

TEST_CASE("White checkmates black king") {
    constexpr Texture2D testTexture{600, 200};
    Board board;

    board.addBothKings();
    board.addPieceToBoard(PieceType::Queen, PieceColor::White, "A1", testTexture);
    board.addPieceToBoard(PieceType::Rook, PieceColor::White, "B7", testTexture);
    board.calculateAllLegalMoves();

    Piece* queen = board.squares.at("A1").piece;
    BoardSquare &target = board.squares.at("A8");

    REQUIRE(queen != nullptr);

    MoveOutcome result = MoveValidator::apply_move(*queen, board, target);

    const bool blackNoLegalMoves = board.getLegalMoveCount(PieceColor::Black) == 0;

    REQUIRE(result.check);
    REQUIRE(queen->square->name == "A8");
    REQUIRE(board.blackIsChecked);
    REQUIRE(blackNoLegalMoves);
}

TEST_CASE("Black can take to avoid checkmate") {
    constexpr Texture2D testTexture{600, 200};
    Board board;

    board.addBothKings();
    board.addPieceToBoard(PieceType::Queen, PieceColor::White, "A1", testTexture);
    board.addPieceToBoard(PieceType::Rook, PieceColor::White, "H7", testTexture);
    board.addPieceToBoard(PieceType::Bishop, PieceColor::Black, "C6", testTexture);
    board.calculateAllLegalMoves();

    Piece* queen = board.squares.at("A1").piece;
    Piece* bishop = board.squares.at("C6").piece;
    BoardSquare &target = board.squares.at("A8");

    REQUIRE(queen != nullptr);

    MoveOutcome result = MoveValidator::apply_move(*queen, board, target);

    bool blackNoLegalMoves = board.getLegalMoveCount(PieceColor::Black) == 0;

    REQUIRE(result.check);
    REQUIRE(queen->square->name == "A8");
    REQUIRE(board.blackIsChecked);
    REQUIRE(!blackNoLegalMoves);
    REQUIRE(bishop->isLegalMove("A8"));

    result = MoveValidator::apply_move(*bishop, board, target);
    blackNoLegalMoves = board.getLegalMoveCount(PieceColor::Black) == 0;
    REQUIRE(!blackNoLegalMoves);
    REQUIRE(bishop->square->name == "A8");
}
