#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "game.h"

void render_game(GameState *game);
void render_hud(GameState *game);
void load_background(GameState *game);
void render_game_over(GameState *game);
void render_menu(GameState *game);
void render_game_objects(GameState *game);

#endif