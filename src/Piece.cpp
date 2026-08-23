#include "raylib.h"
#include <string>
#include "BoardSquare.h"
#include "Piece.h"
#include "Board.h"
#include <iostream>

#pragma region private helpers
std::vector<std::string> calculate_attacking_row(Board *board, const std::string &current, const Piece *piece,
                                                 const int direction) {
    std::vector<std::string> row;
    std::string nextSquare = current;
    const char bound = direction == 1 ? 'H' : 'A';
    while (nextSquare[0] != bound) {
        nextSquare[0] = static_cast<char>(nextSquare[0] + direction);
        if (board->squares[nextSquare].piece) {
            if (board->squares[nextSquare].piece->type == PieceType::King && board->squares[nextSquare].piece->colour !=
                piece->
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

std::vector<std::string> calculate_attacking_column(Board *board, const std::string &current, const Piece *piece,
                                                    const int direction) {
    std::vector<std::string> row;
    std::string nextSquare = current;
    const char bound = direction == 1 ? '8' : '1';
    while (nextSquare[1] != bound) {
        nextSquare[1] = static_cast<char>(nextSquare[1] + direction);
        if (board->squares[nextSquare].piece) {
            if (board->squares[nextSquare].piece->type == PieceType::King && board->squares[nextSquare].piece->colour !=
                piece->
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

std::vector<std::string> calculate_attacking_diagonal(Board *board, const std::string &current, const Piece *piece,
                                                      const int leftOrRight, const int upOrDown) {
    std::vector<std::string> diagonalMoves;
    std::string nextSquare = current;
    const char rowBound = leftOrRight == 1 ? 'H' : 'A';
    const char colBound = upOrDown == 1 ? '8' : '1';

    while (nextSquare[0] != rowBound && nextSquare[1] != colBound) {
        nextSquare[0] = static_cast<char>(nextSquare[0] + leftOrRight);
        nextSquare[1] = static_cast<char>(nextSquare[1] + upOrDown);
        if (board->squares[nextSquare].piece) {
            if (board->squares[nextSquare].piece->type == PieceType::King && board->squares[nextSquare].piece->colour !=
                piece->
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

int Piece::getMaterialValue(const PieceType type) {
    if (type == PieceType::Rook) return 5;
    if (type == PieceType::Queen) return 9;
    if (type == PieceType::Knight) return 3;
    if (type == PieceType::Bishop) return 3;
    return 1;
}

Piece::Piece(const PieceType type, const PieceColor pieceColour, BoardSquare &square,
             const Texture2D &texture) {
    this->type = type;
    this->id = s_id++;
    this->colour = pieceColour;
    this->pieceTexture = getPieceRect(texture, type, pieceColour);
    this->textureDest = square.squareBox;
    this->boundingBox = {square.squareBox.x, square.squareBox.y, BOX_SIZE, BOX_SIZE};
    this->lastPosition = {square.squareBox.x, square.squareBox.y};
    this->square = &square;
    this->value = getMaterialValue(type);
}

void Piece::remove_moves_leading_to_checks(Board *board) {
    for (auto movesIterator = legalMoves.begin(); movesIterator != legalMoves.end();) {
        const auto currentSquare = square;
        bool enpassantTaken = false;
        std::string enpassantSquare;
        Piece *possibleMovableSquaresPiece = nullptr;
        // check if en passant can take attacking piece
        if (std::ranges::count(board->enpassantSquares, board->squares[*movesIterator].name)) {
            const int upOrDownMove = colour == PieceColor::White ? -1 : 1;
            enpassantSquare = {
                (board->squares[*movesIterator].name[0]),
                static_cast<char>(board->squares[*movesIterator].name[1] + upOrDownMove)
            };

            possibleMovableSquaresPiece = board->squares[enpassantSquare].piece;
            board->squares[enpassantSquare].piece->captured = true;
            board->squares[enpassantSquare].piece = nullptr;
            enpassantTaken = true;
        } else {
            possibleMovableSquaresPiece = board->squares[*movesIterator].piece;
            if (possibleMovableSquaresPiece != nullptr) possibleMovableSquaresPiece->captured = true;
        }
        // make phantom move
        square->piece = nullptr;
        square = &board->squares[*movesIterator];
        board->squares[*movesIterator].piece = this;

        // see if king is in check now
        auto newAttackedSquares = board->attackedSquaresOfColor(colour);
        const auto king = board->getKingByColor(colour);
        const bool check = std::ranges::count(newAttackedSquares, king->square->name) >= 1;

        // reset board position
        if (possibleMovableSquaresPiece != nullptr) possibleMovableSquaresPiece->captured = false;
        if (enpassantTaken) {
            board->squares[enpassantSquare].piece = possibleMovableSquaresPiece;
            board->squares[*movesIterator].piece = nullptr;
        } else {
            square->piece = possibleMovableSquaresPiece;
        }

        square = currentSquare;
        square->piece = this;
        if (check) {
            movesIterator = legalMoves.erase(movesIterator);
        } else {
            ++movesIterator;
        }
    }
}

void Piece::printLegalMoves() const {
    for (const auto &move: legalMoves) {
        std::cout << move << " ";
    }
    std::cout << "\n";
}

void Piece::calculate_king_attacking_squares(Board *board) {
    legalMoves.clear();
    attackingSquares.clear();
    const std::string current = square->name;
    const std::string topLeft = {static_cast<char>(current[0] - 1), static_cast<char>(current[1] + 1)};
    const std::string up = {current[0], static_cast<char>(current[1] + 1)};
    const std::string topRight = {static_cast<char>(current[0] + 1), static_cast<char>(current[1] + 1)};
    const std::string right = {static_cast<char>(current[0] + 1), current[1]};
    const std::string downRight = {static_cast<char>(current[0] + 1), static_cast<char>(current[1] - 1)};
    const std::string down = {current[0], static_cast<char>(current[1] - 1)};
    const std::string downLeft = {static_cast<char>(current[0] - 1), static_cast<char>(current[1] - 1)};
    const std::string left = {static_cast<char>(current[0] - 1), current[1]};

    const auto moves = {
        topLeft, up, topRight, right, downRight, down, downLeft, left
    };
    attackingSquares.insert(attackingSquares.end(), moves.begin(), moves.end());
}

bool Piece::try_promote() {
    if (type != PieceType::Pawn) return false;

    if ((colour == PieceColor::White && square->name[1] == '8') ||
        colour == PieceColor::Black && square->name[1] == '1') {
        type = PieceType::Queen;

        constexpr int pawnIndex = static_cast<int>(PieceType::Pawn);
        constexpr int queenIndex = static_cast<int>(PieceType::Queen);

        pieceTexture = {
            (pieceTexture.x / pawnIndex) * queenIndex,
            pieceTexture.y,
            pieceTexture.width,
            pieceTexture.height
        };
        return true;
    }
    return false;
}

void Piece::calculateLegalMoves(Board *board) {
    legalMoves.clear();
    attackingSquares.clear();
    const std::string current = square->name;
    if (captured) return;

    if (type == PieceType::Pawn) {
        const int upOrDownMove = colour == PieceColor::White ? UP : DOWN;

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
        if ((board->isPossibleMove(leftTake) && board->squares[leftTake].piece && board->squares[leftTake].piece->colour
             !=
             colour) || std::ranges::count(board->enpassantSquares, leftTake)) {
            legalMoves.push_back(leftTake);
        }
        if ((board->isPossibleMove(rightTake) && board->squares[rightTake].piece && board->squares[rightTake].piece->
             colour
             != colour) || std::ranges::count(board->enpassantSquares, rightTake)) {
            legalMoves.push_back(rightTake);
        }
        if (board->isPossibleMove(upOne) && !board->squares[upOne].piece) {
            legalMoves.push_back(upOne);
            if (board->isPossibleMove(upTwo) && !board->squares[upTwo].piece && !hasMoved)
                legalMoves.push_back(upTwo);
        }
    }

    if (type == PieceType::Knight) {
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
            } else if (board->squares[*movesIterator].piece != nullptr && board->squares[*movesIterator].piece->colour
                       ==
                       colour) {
                movesIterator = legalMoves.erase(movesIterator);
            } else {
                ++movesIterator;
            }
        }
    }

    if (type == PieceType::Rook) {
        std::vector<std::string> rightRow = calculate_attacking_row(board, current, this, RIGHT);
        std::vector<std::string> leftRow = calculate_attacking_row(board, current, this, LEFT);
        std::vector<std::string> upCol = calculate_attacking_column(board, current, this, UP);
        std::vector<std::string> downCol = calculate_attacking_column(board, current, this, DOWN);

        legalMoves.insert(legalMoves.end(), rightRow.begin(), rightRow.end());
        legalMoves.insert(legalMoves.end(), leftRow.begin(), leftRow.end());
        legalMoves.insert(legalMoves.end(), upCol.begin(), upCol.end());
        legalMoves.insert(legalMoves.end(), downCol.begin(), downCol.end());
    }

    if (type == PieceType::Bishop) {
        std::vector<std::string> topLeftDiagonal = calculate_attacking_diagonal(board, current, this, LEFT, UP);
        std::vector<std::string> topRightDiagonal = calculate_attacking_diagonal(board, current, this, RIGHT, UP);
        std::vector<std::string> bottomLeftDiagonal = calculate_attacking_diagonal(board, current, this, LEFT, DOWN);
        std::vector<std::string> bottomRightDiagonal = calculate_attacking_diagonal(board, current, this, RIGHT, DOWN);

        legalMoves.insert(legalMoves.end(), topRightDiagonal.begin(), topRightDiagonal.end());
        legalMoves.insert(legalMoves.end(), topLeftDiagonal.begin(), topLeftDiagonal.end());
        legalMoves.insert(legalMoves.end(), bottomRightDiagonal.begin(), bottomRightDiagonal.end());
        legalMoves.insert(legalMoves.end(), bottomLeftDiagonal.begin(), bottomLeftDiagonal.end());
    }

    if (type == PieceType::Queen) {
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

    if (type == PieceType::King) {
        const std::string topLeft = {static_cast<char>(current[0] - 1), static_cast<char>(current[1] + 1)};
        const std::string up = {current[0], static_cast<char>(current[1] + 1)};
        const std::string topRight = {static_cast<char>(current[0] + 1), static_cast<char>(current[1] + 1)};
        const std::string right = {static_cast<char>(current[0] + 1), current[1]};
        const std::string downRight = {static_cast<char>(current[0] + 1), static_cast<char>(current[1] - 1)};
        const std::string down = {current[0], static_cast<char>(current[1] - 1)};
        const std::string downLeft = {static_cast<char>(current[0] - 1), static_cast<char>(current[1] - 1)};
        const std::string left = {static_cast<char>(current[0] - 1), current[1]};

        legalMoves = {
            topLeft, up, topRight, right, downRight, down, downLeft, left
        };

        auto attackedSquares = board->attackedSquaresOfColor(colour);

        if (!hasMoved && !board->isColourChecked(colour)) {
            if (colour == PieceColor::White) {
                if (board->whiteCanShortCastle && board->is_square_empty("F1") && board->is_square_empty("G1") &&
                    std::ranges::count(attackedSquares, "F1") == 0) {
                    legalMoves.emplace_back("G1");
                }
                if (board->whiteCanLongCastle && board->is_square_empty("B1") && board->is_square_empty("C1") && board->
                    is_square_empty("D1") && std::ranges::count(attackedSquares, "D1") == 0) {
                    legalMoves.emplace_back("C1");
                }
            }
            if (colour == PieceColor::Black) {
                if (board->blackCanShortCastle && board->is_square_empty("F8") && board->is_square_empty("G8") &&
                    std::ranges::count(attackedSquares, "F8") == 0) {
                    legalMoves.emplace_back("G8");
                }
                if (board->blackCanLongCastle && board->is_square_empty("B8") && board->is_square_empty("C8") && board->
                    is_square_empty("D8") && std::ranges::count(attackedSquares, "D8") == 0) {
                    legalMoves.emplace_back("C8");
                }
            }
        }
        attackingSquares.insert(attackingSquares.end(), legalMoves.begin(), legalMoves.end());


        for (auto movesIterator = legalMoves.begin(); movesIterator != legalMoves.end();) {
            if (std::ranges::count(board->possibleMoves, *movesIterator) != 1) {
                movesIterator = legalMoves.erase(movesIterator);
            } else if (board->squares[*movesIterator].piece != nullptr && board->squares[*movesIterator].piece->colour
                       ==
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
    if (!captured) {
        DrawTexturePro(piecesTexture, pieceTexture, textureDest, {0, 0}, 0, WHITE);
    } else {
        DrawTexturePro(piecesTexture, pieceTexture, textureDest, {0, 0}, 0, BLANK);
    }
}

void Piece::reset_position() {
    setCurrentPos({square->squareBox.x, square->squareBox.y});
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
    return type != PieceType::King && type != PieceType::Pawn;
}

char Piece::getPieceNotation() const {
    if (type == PieceType::King) return 'K';
    if (type == PieceType::Rook) return 'R';
    if (type == PieceType::Queen) return 'Q';
    if (type == PieceType::Knight) return 'N';
    if (type == PieceType::Bishop) return 'B';
    return ' ';
}

Rectangle Piece::getPieceRect(const Texture2D &piecesTexture, const PieceType piece, const PieceColor colour) {
    constexpr int cols = 6;
    constexpr int rows = 2;

    const int pieceWidth = piecesTexture.width / cols;
    const int pieceHeight = piecesTexture.height / rows;

    const int pieceIndex = static_cast<int>(piece);
    const int colourIndex = static_cast<int>(colour);

    return {
        static_cast<float>(pieceIndex * pieceWidth),
        static_cast<float>(colourIndex * pieceHeight),
        static_cast<float>(pieceWidth),
        static_cast<float>(pieceHeight)
    };
}
