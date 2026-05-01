#include "raylib.h"
#include <string>
#include "BoardSquare.h"
#include "Piece.h"

#pragma region private helpers
std::vector<std::string> calculate_attacking_row(Board* board, const std::string &current, const Piece *piece,
                                                 const int direction) {
    std::vector<std::string> row;
    std::string nextSquare = current;
    const char bound = direction == 1 ? 'H' : 'A';
    while (nextSquare[0] != bound) {
        nextSquare[0] = static_cast<char>(nextSquare[0] + direction);
        if (board->squares[nextSquare].piece) {
            if (board->squares[nextSquare].piece->type == "king" && board->squares[nextSquare].piece->colour != piece->
                colour) {
                row.push_back(nextSquare);
                continue;
            }
            row.push_back(nextSquare);
            break;
        }
        row.push_back(nextSquare);
    }
    return row;
}

std::vector<std::string> calculate_attacking_column(Board* board, const std::string &current, const Piece *piece,
                                                    const int direction) {
    std::vector<std::string> row;
    std::string nextSquare = current;
    const char bound = direction == 1 ? '8' : '1';
    while (nextSquare[1] != bound) {
        nextSquare[1] = static_cast<char>(nextSquare[1] + direction);
        if (board->squares[nextSquare].piece) {
            if (board->squares[nextSquare].piece->type == "king" && board->squares[nextSquare].piece->colour != piece->
                colour) {
                row.push_back(nextSquare);
                continue;
            }
            row.push_back(nextSquare);
            break;
        }
        row.push_back(nextSquare);
    }
    return row;
}

std::vector<std::string> calculate_attacking_diagonal(Board* board, const std::string &current, const Piece *piece,
                                                      const int leftOrRight, const int upOrDown) {
    std::vector<std::string> diagonalMoves;
    std::string nextSquare = current;
    const char rowBound = leftOrRight == 1 ? 'H' : 'A';
    const char colBound = upOrDown == 1 ? '8' : '1';

    while (nextSquare[0] != rowBound && nextSquare[1] != colBound) {
        nextSquare[0] = static_cast<char>(nextSquare[0] + leftOrRight);
        nextSquare[1] = static_cast<char>(nextSquare[1] + upOrDown);
        if (board->squares[nextSquare].piece) {
            if (board->squares[nextSquare].piece->type == "king" && board->squares[nextSquare].piece->colour != piece->
                colour) {
                diagonalMoves.push_back(nextSquare);
                continue;
            }
            diagonalMoves.push_back(nextSquare);
            break;
        }
        diagonalMoves.push_back(nextSquare);
    }

    return diagonalMoves;
}
#pragma endregion private helpers

Piece::Piece(const std::string &type, const int pieceSprite, const int pieceColour, BoardSquare &square,
             const Texture2D &texture) {
    this->type = type;
    this->id = s_id++;
    this->colour = pieceColour;
    this->pieceTexture = getPieceRect(texture, pieceSprite, pieceColour);
    this->textureDest = square.squareBox;
    this->boundingBox = {square.squareBox.x, square.squareBox.y, BOX_SIZE, BOX_SIZE};
    this->lastPosition = {square.squareBox.x, square.squareBox.y};
    this->square = &square;
}

void Piece::calculateLegalMoves(const std::vector<Piece> &pieceList, Board* board) {
    legalMoves.clear();
    attackingSquares.clear();
    const std::string current = square->name;
    if (type == "pawn") {
        const int upOrDownMove = colour == PIECE_WHITE ? UP : DOWN;

        std::string upOne = {static_cast<char>(current[0]), static_cast<char>(current[1] + upOrDownMove)};
        std::string upTwo = {static_cast<char>(current[0]), static_cast<char>(upOne[1] + upOrDownMove)};
        std::string leftTake = {static_cast<char>(current[0] - 1), upOne[1]};
        std::string rightTake = {static_cast<char>(current[0] + 1), upOne[1]};

        if (board->isPossibleMove(leftTake)) {
            attackingSquares.push_back(leftTake);
        }
        if (board->isPossibleMove(rightTake)) {
            attackingSquares.push_back(rightTake);
        }
        if (board->isPossibleMove(leftTake) && board->squares[leftTake].piece && board->squares[leftTake].piece->colour !=
            colour) {
            legalMoves.push_back(leftTake);
        }
        if (board->isPossibleMove(rightTake) && board->squares[rightTake].piece && board->squares[rightTake].piece->colour
            != colour) {
            legalMoves.push_back(rightTake);
        }
        if (!board->squares[upOne].piece) {
            legalMoves.push_back(upOne);
            if (!board->squares[upTwo].piece && !hasMoved)
                legalMoves.push_back(upTwo);
        }
    }

    if (type == "knight") {
        std::string upTwoLeftOne = {static_cast<char>(current[0] - 1), static_cast<char>(current[1] + 2)};
        std::string upTwoRightOne = {static_cast<char>(current[0] + 1), static_cast<char>(current[1] + 2)};
        std::string upOneLeftTwo = {static_cast<char>(current[0] - 2), static_cast<char>(current[1] + 1)};
        std::string upOneRightTwo = {static_cast<char>(current[0] + 2), static_cast<char>(current[1] + 1)};
        std::string downOneLeftTwo = {static_cast<char>(current[0] - 2), static_cast<char>(current[1] - 1)};
        std::string downOneRightTwo = {static_cast<char>(current[0] + 2), static_cast<char>(current[1] - 1)};
        std::string downTwoLeftOne = {static_cast<char>(current[0] - 1), static_cast<char>(current[1] - 2)};
        std::string downTwoRightOne = {static_cast<char>(current[0] + 1), static_cast<char>(current[1] - 2)};

        legalMoves = {
            upTwoLeftOne, upTwoRightOne, upOneLeftTwo, upOneRightTwo, downOneLeftTwo, downOneRightTwo, downTwoLeftOne,
            downTwoRightOne
        };
        attackingSquares.insert(attackingSquares.end(), legalMoves.begin(), legalMoves.end());

        for (auto movesIterator = legalMoves.begin(); movesIterator != legalMoves.end();) {
            if (std::ranges::count(board->possibleMoves, *movesIterator) != 1) {
                movesIterator = legalMoves.erase(movesIterator);
            } else if (board->squares[*movesIterator].piece != nullptr && board->squares[*movesIterator].piece->colour ==
                       colour) {
                movesIterator = legalMoves.erase(movesIterator);
            } else {
                ++movesIterator;
            }
        }
    }

    if (type == "rook") {
        std::vector<std::string> rightRow = calculate_attacking_row(board, current, this, RIGHT);
        std::vector<std::string> leftRow = calculate_attacking_row(board, current, this, LEFT);
        std::vector<std::string> upCol = calculate_attacking_column(board, current, this, UP);
        std::vector<std::string> downCol = calculate_attacking_column(board, current, this, DOWN);

        legalMoves.insert(legalMoves.end(), rightRow.begin(), rightRow.end());
        legalMoves.insert(legalMoves.end(), leftRow.begin(), leftRow.end());
        legalMoves.insert(legalMoves.end(), upCol.begin(), upCol.end());
        legalMoves.insert(legalMoves.end(), downCol.begin(), downCol.end());
    }

    if (type == "bishop") {
        std::vector<std::string> topLeftDiagonal = calculate_attacking_diagonal(board, current, this, LEFT, UP);
        std::vector<std::string> topRightDiagonal = calculate_attacking_diagonal(board, current, this, RIGHT, UP);
        std::vector<std::string> bottomLeftDiagonal = calculate_attacking_diagonal(board, current, this, LEFT, DOWN);
        std::vector<std::string> bottomRightDiagonal = calculate_attacking_diagonal(board, current, this, RIGHT, DOWN);

        legalMoves.insert(legalMoves.end(), topRightDiagonal.begin(), topRightDiagonal.end());
        legalMoves.insert(legalMoves.end(), topLeftDiagonal.begin(), topLeftDiagonal.end());
        legalMoves.insert(legalMoves.end(), bottomRightDiagonal.begin(), bottomRightDiagonal.end());
        legalMoves.insert(legalMoves.end(), bottomLeftDiagonal.begin(), bottomLeftDiagonal.end());
    }

    if (type == "queen") {
        std::vector<std::string> rightRow = calculate_attacking_row(board, current, this, RIGHT);
        std::vector<std::string> leftRow = calculate_attacking_row(board, current, this, LEFT);
        std::vector<std::string> upCol = calculate_attacking_column(board, current, this, UP);
        std::vector<std::string> downCol = calculate_attacking_column(board, current, this, DOWN);

        std::vector<std::string> topLeftDiagonal = calculate_attacking_diagonal(board, current, this, LEFT, UP);
        std::vector<std::string> topRightDiagonal = calculate_attacking_diagonal(board, current, this, RIGHT, UP);
        std::vector<std::string> bottomLeftDiagonal = calculate_attacking_diagonal(board, current, this, LEFT, DOWN);
        std::vector<std::string> bottomRightDiagonal = calculate_attacking_diagonal(board, current, this, RIGHT, DOWN);

        legalMoves.insert(legalMoves.end(), topRightDiagonal.begin(), topRightDiagonal.end());
        legalMoves.insert(legalMoves.end(), topLeftDiagonal.begin(), topLeftDiagonal.end());
        legalMoves.insert(legalMoves.end(), bottomRightDiagonal.begin(), bottomRightDiagonal.end());
        legalMoves.insert(legalMoves.end(), bottomLeftDiagonal.begin(), bottomLeftDiagonal.end());

        legalMoves.insert(legalMoves.end(), rightRow.begin(), rightRow.end());
        legalMoves.insert(legalMoves.end(), leftRow.begin(), leftRow.end());
        legalMoves.insert(legalMoves.end(), upCol.begin(), upCol.end());
        legalMoves.insert(legalMoves.end(), downCol.begin(), downCol.end());
    }

    if (type == "king") {
        std::string topLeft = {static_cast<char>(current[0] - 1), static_cast<char>(current[1] + 1)};
        std::string up = {current[0], static_cast<char>(current[1] + 1)};
        std::string topRight = {static_cast<char>(current[0] + 1), static_cast<char>(current[1] + 1)};
        std::string right = {static_cast<char>(current[0] + 1), current[1]};
        std::string downRight = {static_cast<char>(current[0] + 1), static_cast<char>(current[1] - 1)};
        std::string down = {current[0], static_cast<char>(current[1] - 1)};
        std::string downLeft = {static_cast<char>(current[0] - 1), static_cast<char>(current[1] - 1)};
        std::string left = {static_cast<char>(current[0] - 1), current[1]};

        legalMoves = {
            topLeft, up, topRight, right, downRight, down, downLeft, left
        };
        attackingSquares.insert(attackingSquares.end(), legalMoves.begin(), legalMoves.end());

        auto attackedSquares = Board::attackedSquaresOfColor(pieceList, colour);

        for (auto movesIterator = legalMoves.begin(); movesIterator != legalMoves.end();) {
            if (std::ranges::count(board->possibleMoves, *movesIterator) != 1) {
                movesIterator = legalMoves.erase(movesIterator);
            } else if (board->squares[*movesIterator].piece != nullptr && board->squares[*movesIterator].piece->colour ==
                       colour) {
                movesIterator = legalMoves.erase(movesIterator);
            } else if (std::ranges::count(attackedSquares, *movesIterator) >= 1) {
                movesIterator = legalMoves.erase(movesIterator);
            } else {
                ++movesIterator;
            }
        }
    }
}

bool Piece::isLegalMove(const std::string &move) {
    return std::ranges::count(legalMoves, move) == 1;
}

void Piece::Draw(const Texture2D &piecesTexture) const {
    if (!taken) {
        DrawTexturePro(piecesTexture, pieceTexture, textureDest, {0, 0}, 0, WHITE);
    } else {
        DrawTexturePro(piecesTexture, pieceTexture, textureDest, {0, 0}, 0, BLANK);
    }
}

[[nodiscard]]
Vector2 Piece::getCurrentPos() const {
    return {boundingBox.x, boundingBox.y};
}

void Piece::setCurrentPos(const Vector2 pos) {
    boundingBox.x = pos.x;
    boundingBox.y = pos.y;
    textureDest.x = pos.x;
    textureDest.y = pos.y;
}

bool Piece::notPawnOrKing() const {
    return type != "king" && type != "pawn";
}

Rectangle Piece::getPieceRect(const Texture2D &piecesTexture, const int piece, const int colour) {
    constexpr int cols = 6;
    constexpr int rows = 2;
    const int pieceWidth = piecesTexture.width / cols;
    const int pieceHeight = piecesTexture.height / rows;

    return {
        static_cast<float>(piece * pieceWidth),
        static_cast<float>(colour * pieceHeight),
        static_cast<float>(pieceWidth),
        static_cast<float>(pieceHeight)
    };
}
