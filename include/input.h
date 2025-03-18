#ifndef INPUT_H
#define INPUT_H

#include "game.h"
#include "audio.h" 

void handle_input(GameState *game, SoundEffects *sounds);
void toggle_fullscreen(GameState *game);

#endif