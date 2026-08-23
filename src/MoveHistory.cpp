//
// Created by papie on 19/08/2026.
//

#include "MoveHistory.h"
#include <iostream>
#include "raylib.h"

void MoveHistory::draw() const {
    constexpr int border = 5;
    const int moves_count = movesList.size();

    int move_offset = 0;
    if (moves_count > MOVES_DRAW_LIMIT) {
        move_offset = ((moves_count / MOVES_DRAW_LIMIT) - 1) * MOVES_DRAW_LIMIT;
        move_offset += (moves_count % MOVES_DRAW_LIMIT);
        move_offset % 2 == 0 ? move_offset : move_offset++; // round offset to even number
    }

    DrawRectangle(750, 75, 300, 600, BLACK);
    DrawRectangle(750 + border, 75 + border, 300 - border * 2, 600 - border * 2, GRAY);
    int text_offset_y = 0;
    int text_offset_x = 0;

    for (size_t i = move_offset; i < moves_count; i++) {
        const int move = (i / 2) + 1;
        if (i % 2 == 0) {
            auto number_str = std::to_string(move) + ".";
            DrawText(number_str.c_str(), 790, 100 + text_offset_y, 20, BLACK);
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
    for (const auto& move : movesList) {
        std::cout << move << " ";
    }
    std::cout << "\n";
}

void MoveHistory::clear_history() {
    movesList.clear();
}
