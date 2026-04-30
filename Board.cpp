#include "Board.h"
#include "Piece.h"

Board::Board() {
    possibleMoves.reserve(64);
    for (int i = 1; i <= 8; i++) {
        for (int j = 1; j <= 8; j++) {
            std::string letter = numToLetter[j];
            std::string number = std::to_string(abs(i - 9));
            squares[letter + number] = {
                SQUARE_SIZE * static_cast<float>(j), SQUARE_SIZE * static_cast<float>(i), SQUARE_SIZE, SQUARE_SIZE,
                nullptr,
                letter + number
            };
            possibleMoves.push_back(letter + number);
        }
    }
}

void Board::Draw() {
    for (int i = 1; i <= 8; i++) {
        for (int j = 1; j <= 8; j++) {
            const Color colour = (i + j) % 2 == 0 ? WHITE : GRAY;
            DrawRectangle(SQUARE_SIZE * j, SQUARE_SIZE * i, SQUARE_SIZE, SQUARE_SIZE, colour);
        }
    }
    for (int i = 1; i <=8; i++) {
        DrawText(numToLetter[i].c_str(), (SQUARE_SIZE * i) + 30, 680, 16, WHITE);
    }

    for (int i = 1; i <=8; i++) {
        DrawText(std::to_string(abs(i-9)).c_str(), 60, (SQUARE_SIZE * i) + 30, 16, WHITE);
    }
}

void Board::drawLegalMove(const std::string &notation, const int colour) {
    const auto [x, y, width, height] = squares[notation].squareBox;
    if (squares[notation].piece != nullptr && squares[notation].piece->colour != colour) {
        DrawCircle(x + width / 2, y + height / 2, MOVE_CIRCLE, {100, 100, 100, 150});
    }
    if (squares[notation].piece == nullptr) {
        DrawCircle(x + width / 2, y + height / 2, MOVE_CIRCLE, {100, 100, 100, 150});
    }
}

bool Board::isPossibleMove(const std::string &move) {
    return std::ranges::count(possibleMoves, move) == 1;
}

std::vector<std::string> Board::attackedSquaresOfColor(const std::vector<Piece> &pieceList, const int colour) {
    std::vector<std::string> allAttackedSquares;
    for (auto& piece: pieceList) {
        if (!piece.taken && piece.colour != colour && piece.notPawnOrKing()) {
            allAttackedSquares.insert(allAttackedSquares.end(), piece.legalMoves.begin(), piece.legalMoves.end());
        }
        if (!piece.taken && piece.colour != colour && !piece.notPawnOrKing()) {
            allAttackedSquares.insert(allAttackedSquares.end(), piece.attackingSquares.begin(), piece.attackingSquares.end());
        }
    }
    return allAttackedSquares;
}
