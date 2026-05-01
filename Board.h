#ifndef CHESSBOT_BOARD_H
#define CHESSBOT_BOARD_H

#include <algorithm>
#include <map>
#include <string>
#include <vector>

#include "BoardSquare.h"

constexpr int SQUARE_SIZE = 75;

static std::map<int, std::string> numToLetter = {
    {1, "A"}, {2, "B"}, {3, "C"}, {4, "D"}, {5, "E"}, {6, "F"}, {7, "G"}, {8, "H"}
};

class Board {
    const float MOVE_CIRCLE = 15;
public:
    std::map<std::string, BoardSquare> squares;
    std::vector<std::string> possibleMoves;
    std::vector<Piece> pieceList;
    Piece* whiteKing{};
    Piece* blackKing{};
    // std::vector<Piece>* pieceList;

    // Board constructor. Initialises the board squares
    Board();

    // Draw the board itself
    static void Draw();

    // Draws the move circle for a legal move
    void drawLegalMove(const std::string &notation, int colour);

    // Checks if the given move is on the board
    bool isPossibleMove(const std::string& move);

    void calculateAllLegalMoves();

    void addPieceToBoard(const std::string &type, int sprite, int colour, const std::string &square, const Texture2D &texture);
    // Gets all the attacked squares for a colour
    static std::vector<std::string> attackedSquaresOfColor(const std::vector<Piece>& pieceList, int colour);
};

#endif //CHESSBOT_BOARD_H