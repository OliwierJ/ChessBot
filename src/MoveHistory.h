#ifndef CHESSBOT_MOVEHISTORY_H
#define CHESSBOT_MOVEHISTORY_H
#include <string>
#include <vector>

constexpr int MOVES_DRAW_LIMIT = 30;

class MoveHistory {

public:
    std::vector<std::string> movesList;
    // Add move notation to the move list
    void append_move(const std::string &notation);

    void print_history() const;

    void clear_history();
};


#endif //CHESSBOT_MOVEHISTORY_H