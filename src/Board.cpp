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
      enpassantSquares(other.enpassantSquares){

    for (const auto& [name, square] : other.squares) {
        squares.emplace(name, square);
        squares.at(name).piece = nullptr;
    }

    for (const auto& original : other.pieceList) {
        if (original.captured) continue;

        pieceList.push_back(original);

        Piece& copy = pieceList.back();
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

void Board::Draw() {
    for (int i = 1; i <= 8; i++) {
        for (int j = 1; j <= 8; j++) {
            const Color colour = (i + j) % 2 == 0 ? WHITE : GRAY;
            DrawRectangle(SQUARE_SIZE * j, SQUARE_SIZE * i, SQUARE_SIZE, SQUARE_SIZE, colour);
        }
    }
    for (int i = 1; i <= 8; i++) {
        DrawText(numToLetter[i].c_str(), (SQUARE_SIZE * i) + 30, 680, 16, WHITE);
    }

    for (int i = 1; i <= 8; i++) {
        DrawText(std::to_string(abs(i - 9)).c_str(), 60, (SQUARE_SIZE * i) + 30, 16, WHITE);
    }
}

void Board::draw_taken_material(const Texture2D &texture) const {
    constexpr Vector2 white_taken = {80, 40};
    constexpr Vector2 black_taken = {80, 700};
    int white_taken_idx = 0;
    int black_taken_idx = 0;
    int white_material_taken = 0;
    int black_material_taken = 0;
    int gap = 20;

    for (int i = 0; i < pieceList.size(); i++) {
        const Piece piece = pieceList.at(i);
        if (piece.captured) {
            // if (i > 0 && piece.type == pieceList.at(i-1).type) gap = 10;

            if (piece.colour == PieceColor::White) {
                black_material_taken += piece.value;
                DrawTexturePro(texture, piece.pieceTexture,
                               {white_taken.x + gap * white_taken_idx, white_taken.y, 25, 25}, {0, 0}, 0, WHITE);
                white_taken_idx++;
            } else {
                DrawTexturePro(texture, piece.pieceTexture,
                               {black_taken.x + gap * black_taken_idx, black_taken.y, 25, 25}, {0, 0}, 0, WHITE);
                white_material_taken += piece.value;
                black_taken_idx++;
            }
        }
    }
    const int white_advantage = white_material_taken - black_material_taken;
    const int black_advantage = black_material_taken - white_material_taken;
    const std::string white_text = "+" + std::to_string(white_advantage);
    const std::string black_text = "+" + std::to_string(black_advantage);
    if (white_advantage > 0) DrawText(white_text.c_str(), black_taken.x + gap * black_taken_idx, black_taken.y + 5, 20, WHITE);
    if (black_advantage > 0) DrawText(black_text.c_str(), white_taken.x + gap * white_taken_idx, white_taken.y + 5, 20, WHITE);
}


void Board::drawLegalMove(const std::string &notation, const PieceColor colour) {
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

std::vector<std::string> Board::attackedSquaresOfColor(const PieceColor colour) {
    std::vector<std::string> allAttackedSquares;
    for (auto &piece: pieceList) {
        if (!piece.captured && piece.colour != colour && piece.notPawnOrKing() && piece.type != PieceType::Knight) {
            piece.calculateLegalMoves(this);
            allAttackedSquares.insert(allAttackedSquares.end(), piece.attackingSquares.begin(), piece.attackingSquares.end());
        }
        if (!piece.captured && piece.colour != colour && piece.type == PieceType::Knight) {
            piece.calculateLegalMoves(this);
            allAttackedSquares.insert(allAttackedSquares.end(), piece.attackingSquares.begin(),
                                      piece.attackingSquares.end());
        }
        if (!piece.captured && piece.colour != colour && !piece.notPawnOrKing()) {
            if (piece.type == PieceType::Pawn) {
                piece.calculateLegalMoves(this);
            }
            if (piece.type == PieceType::King) {
                piece.calculate_king_attacking_squares(this);
            }
            allAttackedSquares.insert(allAttackedSquares.end(), piece.attackingSquares.begin(),
                                      piece.attackingSquares.end());
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
