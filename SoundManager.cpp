//
// Created by papie on 23/08/2026.
//

#include "SoundManager.h"

#include "ChessGame.h"
#include "embedded_resources.h"

Sound SoundManager::checkSound = {};
Sound SoundManager::moveSound = {};
Sound SoundManager::illegalMoveSound = {};
Sound SoundManager::captureSound = {};
Sound SoundManager::promoteSound = {};
Sound SoundManager::gameEndSound = {};

void SoundManager::load_sounds() {
    auto load_sound = [](const char *fileType, const unsigned char *data, const std::size_t size) {
        const Wave wave = LoadWaveFromMemory(fileType, data, static_cast<int>(size));
        const Sound sound = LoadSoundFromWave(wave);
        UnloadWave(wave);
        return sound;
    };

    checkSound = load_sound(".wav", move_check_wav, move_check_wav_size);
    moveSound = load_sound(".wav", move_self_wav, move_self_wav_size);
    illegalMoveSound = load_sound(".wav", illegal_wav, illegal_wav_size);
    captureSound = load_sound(".mp3", capture_mp3, capture_mp3_size);
    promoteSound = load_sound(".mp3", promote_mp3, promote_mp3_size);
    gameEndSound = load_sound(".mp3", game_end_mp3, game_end_mp3_size);
}

void SoundManager::play_move_sound(ChessGame &game, const MoveOutcome result) {
    if (game.state().state == GameStatus::Checkmate ||
        game.state().state == GameStatus::Stalemate) {
        PlaySound(gameEndSound);
    } else if (result.check) {
        PlaySound(checkSound);
    } else if (result.pieceTaken) {
        PlaySound(captureSound);
    } else if (result.pawnPromoted) {
        PlaySound(promoteSound);
    } else {
        PlaySound(moveSound);
    }
}

void SoundManager::unload_sounds() {
    UnloadSound(checkSound);
    UnloadSound(moveSound);
    UnloadSound(illegalMoveSound);
    UnloadSound(captureSound);
    UnloadSound(promoteSound);
    UnloadSound(gameEndSound);
}
