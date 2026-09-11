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
            possibleMoves.insert(letter + number);
        }
    }
}

Board::~Board() {
    for (auto &[square_str, square]: squares) {
        square.piece = nullptr;
    }
}

Board::Board(const Board &other)
    : possibleMoves(other.possibleMoves),
      whiteIsChecked(other.whiteIsChecked),
      blackIsChecked(other.blackIsChecked),
      whiteCanShortCastle(other.whiteCanShortCastle),
      whiteCanLongCastle(other.whiteCanLongCastle),
      blackCanShortCastle(other.blackCanShortCastle),
      blackCanLongCastle(other.blackCanLongCastle),
      enpassantSquares(other.enpassantSquares) {
    for (const auto &[name, square]: other.squares) {
        squares.emplace(name, square);
        squares.at(name).piece = nullptr;
    }

    for (const auto &original: other.pieceList) {
        if (original.captured) continue;

        pieceList.push_back(original);

        Piece &copy = pieceList.back();
        copy.square = &squares.at(original.square->name);
        copy.square->piece = &copy;

        if (original.type == PieceType::King) {
            if (original.colour == PieceColor::White) {
                whiteKing = &pieceList.back();
            } else {
                blackKing = &pieceList.back();
            }
        }
    }
}

void Board::clear_board() {
    for (auto &[square_str, square]: squares) {
        square.piece = nullptr;
    }

    whiteKing = nullptr;
    blackKing = nullptr;
    whiteIsChecked = false;
    blackIsChecked = false;
    pieceList.clear();
}

bool Board::isPossibleMove(const std::string &move) const {
    return possibleMoves.contains(move);
}

void Board::calculateAllLegalMoves() {
    for (auto &p: pieceList) {
        if (p.captured) continue;
        p.calculateLegalMoves(this);
        p.remove_moves_leading_to_checks(this);
    }

    whiteKing->calculateLegalMoves(this);
    blackKing->calculateLegalMoves(this);
}

bool Board::is_square_empty(const std::string &square) {
    return squares[square].piece == nullptr;
}

void Board::calculateAllLegalMovesByColour(const PieceColor colour) {
    for (auto &p: pieceList) {
        if (p.captured || p.colour != colour) continue;
        p.calculateLegalMoves(this);
        p.remove_moves_leading_to_checks(this);
    }
    const auto king = getKingByColor(colour);
    king->calculateLegalMoves(this);
}

void Board::addPieceToBoard(PieceType type, PieceColor colour, const std::string &square, const Texture2D &texture) {
    if (!isPossibleMove(square))
        throw std::invalid_argument(
            "Cannot add piece to illegal square.\nTried to create piece at illegal position " + square
        );

    pieceList.emplace_back(type, colour, this->squares[square], texture);
    squares[square].piece = &pieceList.back();
}

void Board::set_up_pieces(const Texture2D &piecesTexture) {
    // White Pieces
    addPieceToBoard(PieceType::Pawn, PieceColor::White, "A2", piecesTexture);
    addPieceToBoard(PieceType::Pawn, PieceColor::White, "B2", piecesTexture);
    addPieceToBoard(PieceType::Pawn, PieceColor::White, "C2", piecesTexture);
    addPieceToBoard(PieceType::Pawn, PieceColor::White, "D2", piecesTexture);
    addPieceToBoard(PieceType::Pawn, PieceColor::White, "E2", piecesTexture);
    addPieceToBoard(PieceType::Pawn, PieceColor::White, "F2", piecesTexture);
    addPieceToBoard(PieceType::Pawn, PieceColor::White, "G2", piecesTexture);
    addPieceToBoard(PieceType::Pawn, PieceColor::White, "H2", piecesTexture);
    addPieceToBoard(PieceType::Knight, PieceColor::White, "B1", piecesTexture);
    addPieceToBoard(PieceType::Knight, PieceColor::White, "G1", piecesTexture);
    addPieceToBoard(PieceType::Bishop, PieceColor::White, "C1", piecesTexture);
    addPieceToBoard(PieceType::Bishop, PieceColor::White, "F1", piecesTexture);
    addPieceToBoard(PieceType::Rook, PieceColor::White, "A1", piecesTexture);
    addPieceToBoard(PieceType::Rook, PieceColor::White, "H1", piecesTexture);
    addPieceToBoard(PieceType::Queen, PieceColor::White, "D1", piecesTexture);
    addPieceToBoard(PieceType::King, PieceColor::White, "E1", piecesTexture);
    whiteKing = &pieceList.back();
    // Black Pieces
    addPieceToBoard(PieceType::Pawn, PieceColor::Black, "A7", piecesTexture);
    addPieceToBoard(PieceType::Pawn, PieceColor::Black, "B7", piecesTexture);
    addPieceToBoard(PieceType::Pawn, PieceColor::Black, "C7", piecesTexture);
    addPieceToBoard(PieceType::Pawn, PieceColor::Black, "D7", piecesTexture);
    addPieceToBoard(PieceType::Pawn, PieceColor::Black, "E7", piecesTexture);
    addPieceToBoard(PieceType::Pawn, PieceColor::Black, "F7", piecesTexture);
    addPieceToBoard(PieceType::Pawn, PieceColor::Black, "G7", piecesTexture);
    addPieceToBoard(PieceType::Pawn, PieceColor::Black, "H7", piecesTexture);
    addPieceToBoard(PieceType::Knight, PieceColor::Black, "B8", piecesTexture);
    addPieceToBoard(PieceType::Knight, PieceColor::Black, "G8", piecesTexture);
    addPieceToBoard(PieceType::Bishop, PieceColor::Black, "F8", piecesTexture);
    addPieceToBoard(PieceType::Bishop, PieceColor::Black, "C8", piecesTexture);
    addPieceToBoard(PieceType::Rook, PieceColor::Black, "A8", piecesTexture);
    addPieceToBoard(PieceType::Rook, PieceColor::Black, "H8", piecesTexture);
    addPieceToBoard(PieceType::Queen, PieceColor::Black, "D8", piecesTexture);
    addPieceToBoard(PieceType::King, PieceColor::Black, "E8", piecesTexture);
    blackKing = &pieceList.back();


    for (auto &p: pieceList) {
        p.calculateLegalMoves(this);
    }
}

bool Board::isColourChecked(const PieceColor colour) const {
    if (colour == PieceColor::White) return whiteIsChecked;
    return blackIsChecked;
}

Piece *Board::getKingByColor(const PieceColor colour) const {
    if (colour == PieceColor::White) return whiteKing;
    return blackKing;
}

size_t Board::getLegalMoveCount(const PieceColor colour) const {
    size_t count = 0;
    for (const auto &p: pieceList) {
        if (p.colour == colour && !p.captured) {
            count += p.legalMoves.size();
        }
    }
    return count;
}

std::unordered_set<std::string> Board::attackedSquaresOfColor(const PieceColor colour) {
    std::unordered_set<std::string> allAttackedSquares;
    for (auto &piece: pieceList) {
        if (!piece.captured && piece.colour != colour && piece.notPawnOrKing() && piece.type != PieceType::Knight) {
            piece.calculateLegalMoves(this);
            allAttackedSquares.insert(piece.attackingSquares.begin(), piece.attackingSquares.end());
        }
        if (!piece.captured && piece.colour != colour && piece.type == PieceType::Knight) {
            piece.calculateLegalMoves(this);
            allAttackedSquares.insert(piece.attackingSquares.begin(), piece.attackingSquares.end());
        }
        if (!piece.captured && piece.colour != colour && !piece.notPawnOrKing()) {
            if (piece.type == PieceType::Pawn) {
                piece.calculateLegalMoves(this);
            }
            if (piece.type == PieceType::King) {
                piece.calculate_king_attacking_squares(this);
            }
            allAttackedSquares.insert(piece.attackingSquares.begin(), piece.attackingSquares.end());
        }
    }
    return allAttackedSquares;
}

void Board::addBothKings() {
    addPieceToBoard(PieceType::King, PieceColor::Black, "E1", {200, 200});
    whiteKing = &pieceList.back();
    addPieceToBoard(PieceType::King, PieceColor::White, "E8", {200, 200});
    blackKing = &pieceList.back();
}

bool Board::square_contains_opponent_piece(const std::string &square, const PieceColor colour) const {
    if (!isPossibleMove(square)) return false;
    const auto piece = squares.at(square).piece;
    if (piece == nullptr) return false;
    return !piece->captured && piece->colour != colour;
}
