#include "Board.h"

#include <iostream>
#include <stdexcept>

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

void Board::calculateAllLegalMoves() {
    for (auto &p: pieceList) {
        if (p.taken) continue;
        p.calculateLegalMoves(this);
        p.remove_moves_leading_to_checks(this);
        std::cout << p.type << "  " << p.colour << "  ";
        p.printLegalMoves();
    }
    for (auto& p : pieceList) {
        std::cout << p.type << "  " << p.colour << "  ";
        p.printLegalMoves();
    }
    whiteKing->calculateLegalMoves(this);
    blackKing->calculateLegalMoves(this);

}

bool Board::is_square_empty(const std::string &square) {
    return squares[square].piece == nullptr;
}

void Board::calculateAllLegalMovesByColour(const int colour) {
    for (auto &p: pieceList) {
        if (p.taken || p.colour != colour) continue;
        p.calculateLegalMoves(this);
        p.remove_moves_leading_to_checks(this);
        std::cout << p.type << "  " << p.colour << "  ";
        p.printLegalMoves();
    }
    auto king = getKingByColor(colour);
    king->calculateLegalMoves(this);

}

void Board::addPieceToBoard(const std::string& type, const int sprite, const int colour, const std::string &square, const Texture2D& texture) {
    if (!isPossibleMove(square))
        throw std::invalid_argument(
            "Cannot add piece to illegal square.\nTried to create piece " +
            type + " at illegal position " + square
        );

    pieceList.emplace_back(type, sprite, colour, this->squares[square], texture);
    squares[square].piece = &pieceList.back();
}

bool Board::isColourChecked(const int colour) const {
    if (colour == 0) return whiteIsChecked;
    return blackIsChecked;
}

Piece * Board::getKingByColor(const int colour) const {
    if (colour == 0) return whiteKing;
    return blackKing;
}

size_t Board::getLegalMoveCount(const int colour) const {
    size_t count = 0;
    for (const auto& p : pieceList) {
        if (p.colour == colour && !p.taken) {
            count += p.legalMoves.size();
        }
    }
    return count;
}
std::vector<std::string> Board::attackedSquaresOfColor(const int colour) {
    std::vector<std::string> allAttackedSquares;
    for (auto& piece: pieceList) {
        if (!piece.taken && piece.colour != colour && piece.notPawnOrKing()) {
            piece.calculateLegalMoves(this);
            allAttackedSquares.insert(allAttackedSquares.end(), piece.legalMoves.begin(), piece.legalMoves.end());
        }
        if (!piece.taken && piece.colour != colour && !piece.notPawnOrKing()) {
            if (piece.type == "pawn") {
                piece.calculateLegalMoves(this);
            }
            if (piece.type == "king") {
                piece.calculate_king_attacking_squares(this);
            }
            allAttackedSquares.insert(allAttackedSquares.end(), piece.attackingSquares.begin(), piece.attackingSquares.end());
        }
    }
    return allAttackedSquares;
}
