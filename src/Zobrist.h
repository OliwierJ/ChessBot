#ifndef CHESSBOT_ZOBRIST_H
#define CHESSBOT_ZOBRIST_H
#include <cstdint>

class Zobrist {
public:
    static uint64_t pieces_hash[2][6][64];
    static uint64_t side_to_move_hash;
    static uint64_t castling_hash[4];
    static uint64_t enpassant_file_hash[8];
};


#endif //CHESSBOT_ZOBRIST_H