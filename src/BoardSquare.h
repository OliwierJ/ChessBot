#ifndef CHESSBOT_BOARDSQUARE_H
#define CHESSBOT_BOARDSQUARE_H
#pragma once
#include <string>
#include "raylib.h"

class Piece;

class BoardSquare {
public:
    Rectangle squareBox;
    Piece* piece = nullptr;
    std::string name;
};


#endif