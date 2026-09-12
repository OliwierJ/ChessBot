#include "Zobrist.h"

uint64_t Zobrist::pieces_hash[2][6][64];
uint64_t Zobrist::side_to_move_hash;
uint64_t Zobrist::castling_hash[4];
uint64_t Zobrist::enpassant_file_hash[8];