#ifndef CHESSBOT_PIECE_H
#define CHESSBOT_PIECE_H

#include "raylib.h"
#include <string>
#include <vector>
#include "Board.h"
#include "BoardSquare.h"

static int s_id = 0;
constexpr float BOX_SIZE = 60;

enum Pieces { KING, QUEEN, BISHOP, KNIGHT, ROOK, PAWN };
enum PieceColor { PIECE_WHITE, PIECE_BLACK };

class Piece {
public:
    Rectangle pieceTexture{};
    Rectangle textureDest{};
    Rectangle boundingBox{};
    Vector2 lastPosition{};
    int id;
    int colour;
    bool isCurrentlyHeld = false;
    bool taken = false;
    BoardSquare *square;
    std::vector<std::string> legalMoves;
    std::vector<std::string> attackingSquares;
    std::string type;
    bool hasMoved = false;

    Piece(const std::string &type, int pieceSprite, int pieceColour, BoardSquare &square, const Texture2D &texture);

    void remove_moves_leading_to_checks(Board *board);

    void printLegalMoves() const;

    void calculate_king_attacking_squares(Board *board);

    bool try_promote();

    void calculateLegalMoves(Board *board);

    bool isLegalMove(const std::string& move);

    void Draw(const Texture2D &piecesTexture) const;

    [[nodiscard]]
    Vector2 getCurrentPos() const;

    void setCurrentPos(Vector2 pos);

    [[nodiscard]]
     bool notPawnOrKing() const;

private:
    const int UP = 1;
    const int DOWN = -1;
    const int RIGHT = 1;
    const int LEFT = -1;
    static Rectangle getPieceRect(const Texture2D &piecesTexture, int piece, int colour);
};

#endif
