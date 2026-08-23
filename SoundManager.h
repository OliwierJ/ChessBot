//
// Created by papie on 23/08/2026.
//

#ifndef CHESSBOT_SOUNDMANAGER_H
#define CHESSBOT_SOUNDMANAGER_H
#include "MoveValidator.h"
#include "raylib.h"


class ChessGame;

class SoundManager {
public:
    static void load_sounds();

    static void play_move_sound(ChessGame &game, MoveOutcome result);

    static void unload_sounds();

private:
    static Sound checkSound;
    static Sound moveSound;
    static Sound illegalMoveSound;
    static Sound captureSound;
    static Sound promoteSound;
    static Sound gameEndSound;
};


#endif //CHESSBOT_SOUNDMANAGER_H