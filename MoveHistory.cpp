//
// Created by papie on 19/08/2026.
//

#include "MoveHistory.h"

#include <iostream>

#include "raylib.h"

void MoveHistory::draw() const {
    constexpr int offset = 5;
    DrawRectangle(750, 75, 300, 600, BLACK);
    DrawRectangle(750 + offset, 75 + offset, 300 - offset * 2, 600 - offset * 2, GRAY);
    int text_offset_y = 0;
    int text_offset_x = 0;

    for (size_t i = 0; i < movesList.size(); i++) {
        const int move = (i / 2) + 1;
        if (i % 2 == 0) {
            DrawText(std::to_string(move).c_str(), 800, 100 + text_offset_y, 20, BLACK);
            DrawText(movesList[i].c_str(), 850 + text_offset_x, 100 + text_offset_y, 20, BLACK);
            text_offset_x = 100;
        } else {
            DrawText(movesList[i].c_str(), 850 + text_offset_x, 100 + text_offset_y, 20, BLACK);
            text_offset_y += 30;
            text_offset_x = 0;
        }
    }
}

void MoveHistory::append_move(const std::string &notation) {
    movesList.push_back(notation);
}

void MoveHistory::print_history() const {
    std::cout << "Move List: \n";
    for (auto move : movesList) {
        std::cout << move << " ";
    }
    std::cout << "\n";
}
