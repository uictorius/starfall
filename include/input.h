#ifndef INPUT_H
#define INPUT_H

#include "game.h"
#include "audio.h" 

void handle_input(GameState *game);
void toggle_fullscreen(GameState *game);
void handle_playing_input(GameState *game, SDL_Event *event);
void handle_menu_input(GameState *game, SDL_Event *event);
void handle_game_over_input(GameState *game, SDL_Event *event);
void handle_game_over_selection(GameState *game);
void handle_game_over_mouse(GameState *game, int mouseX, int mouseY);

#endif