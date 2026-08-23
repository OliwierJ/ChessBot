#ifndef CHESSBOT_BOARD_H
#define CHESSBOT_BOARD_H

#include <algorithm>
#include <deque>
#include <map>
#include <string>
#include <vector>

#include "BoardSquare.h"
#include "Piece.h"

constexpr int SQUARE_SIZE = 75;

static std::map<int, std::string> numToLetter = {
    {1, "A"}, {2, "B"}, {3, "C"}, {4, "D"}, {5, "E"}, {6, "F"}, {7, "G"}, {8, "H"}
};

class Board {
    const float MOVE_CIRCLE = 15;
public:
    std::map<std::string, BoardSquare> squares;
    std::vector<std::string> possibleMoves;
    std::deque<Piece> pieceList;
    Piece* whiteKing{};
    Piece* blackKing{};
    bool whiteIsChecked = false;
    bool blackIsChecked = false;
    // std::vector<Piece>* pieceList;

    // Board constructor. Initialises the board squares
    Board();

    // Draw the board itself
    static void Draw();

    // Draws the move circle for a legal move
    void drawLegalMove(const std::string &notation, PieceColor colour);

    // Checks if the given move is on the board
    bool isPossibleMove(const std::string& move);

    // calculates all legal moves for all pieces, followed by both kings
    void calculateAllLegalMoves();

    bool is_square_empty(const std::string &square);

    void calculateAllLegalMovesByColour(PieceColor colour);

    // Creates a piece and adds it to the board square and piece list
    void addPieceToBoard(PieceType type, PieceColor colour, const std::string &square, const Texture2D &texture);

    void set_up_pieces(const Texture2D &piecesTexture);

    bool whiteCanShortCastle = true;
    bool whiteCanLongCastle = true;
    bool blackCanShortCastle = true;
    bool blackCanLongCastle = true;

    std::vector<std::string> enpassantSquares = {};

    [[nodiscard]]
    bool isColourChecked(PieceColor colour) const;

    Piece* getKingByColor(PieceColor colour) const;

    size_t getLegalMoveCount(PieceColor colour) const;

    // Gets all the attacked squares for a colour
    std::vector<std::string> attackedSquaresOfColor(PieceColor colour);

    void addBothKings();
};

#endif //CHESSBOT_BOARD_H