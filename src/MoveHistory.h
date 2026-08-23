//
// Created by papie on 19/08/2026.
//

#ifndef CHESSBOT_MOVEHISTORY_H
#define CHESSBOT_MOVEHISTORY_H
#include <string>
#include <vector>

constexpr int MOVES_DRAW_LIMIT = 30;

class MoveHistory {
    std::vector<std::string> movesList;

public:

    // Draw the moves list box
    void draw() const;

    // Add move notation to the move list
    void append_move(const std::string &notation);

    void print_history() const;

    void clear_history();
};


#endif //CHESSBOT_MOVEHISTORY_H