#ifndef AUDIO_H
#define AUDIO_H

#include "game.h"

int init_audio();
void load_sounds(GameState *game);
void load_music(GameState *game);
void cleanup_audio(GameState *game);

#endif