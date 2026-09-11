//
// Created by papie on 19/08/2026.
//

#include "MoveHistory.h"
#include <iostream>

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
