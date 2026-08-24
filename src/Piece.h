#ifndef CHESSBOT_PIECE_H
#define CHESSBOT_PIECE_H

#include "raylib.h"
#include <string>
#include <vector>

class Board;
class BoardSquare;

static int s_id = 0;
constexpr float BOX_SIZE = 60;

enum class PieceType {
    King = 0,
    Queen = 1,
    Bishop = 2,
    Knight = 3,
    Rook = 4,
    Pawn = 5
};

enum class PieceColor {
    White = 0,
    Black = 1
};

constexpr PieceColor opposite(const PieceColor colour) {
    return colour == PieceColor::White
        ? PieceColor::Black
        : PieceColor::White;
}

class Piece {
public:
    Rectangle pieceTexture{};
    Rectangle textureDest{};
    Rectangle boundingBox{};
    Vector2 lastPosition{};
    int id;
    PieceColor colour;
    bool isCurrentlyHeld = false;
    bool captured = false;
    bool hasMoved = false;
    BoardSquare *square;
    std::vector<std::string> legalMoves;
    std::vector<std::string> attackingSquares;
    PieceType type;
    int value;

    void calculate_attacking_row(Board *board, const std::string &current, const Piece *piece, int direction);

    void calculate_attacking_column(Board *board, const std::string &current, const Piece *piece, int direction);

    void calculate_attacking_diagonal(Board *board, const std::string &current, const Piece *piece, int leftOrRight,
                                      int upOrDown);

    static int getMaterialValue(PieceType type);

    Piece(PieceType type, PieceColor pieceColour, BoardSquare &square, const Texture2D &texture);

    void remove_moves_leading_to_checks(Board *board);

    void printLegalMoves() const;

    void calculate_king_attacking_squares(Board *board);

    bool try_promote();

    void calculateLegalMoves(Board *board);

    bool isLegalMove(const std::string& move);

    void Draw(const Texture2D &piecesTexture) const;

    void reset_position();

    [[nodiscard]]
    Vector2 getCurrentPos() const;

    void setCurrentPos(Vector2 pos);

    [[nodiscard]]
    bool notPawnOrKing() const;

    [[nodiscard]]
    char getPieceNotation() const;

    std::string to_string() const;

private:
    const int UP = 1;
    const int DOWN = -1;
    const int RIGHT = 1;
    const int LEFT = -1;
    static Rectangle getPieceRect(const Texture2D &piecesTexture, PieceType piece, PieceColor colour);
};

#endif
