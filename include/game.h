#ifndef GAME_H
#define GAME_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdbool.h>
#include "config.h"

typedef struct GameState GameState;
#include "player.h"
#include "projectile.h"
#include "enemy.h"

struct GameState {
    int current_width;
    int current_height;
    int stored_window_width;
    int stored_window_height;
    SDL_Window *window;
    SDL_Renderer *renderer;
    TTF_Font *font;
    Player player;
    Projectile projectiles[MAX_PROJECTILES];
    Enemy enemies[MAX_ENEMIES];
    int score;
    bool running;
    bool is_fullscreen;
};

// Declarações de funções
void initialize_game(GameState *game);
void run_game_loop(GameState *game);
void cleanup_game(GameState *game);

// Define os paths relativos
#define ASSETS_DIR "assets"
#define FONTS_DIR ASSETS_DIR "/fonts"
#define IMAGES_DIR ASSETS_DIR "/images"
#define FONT_MAIN FONTS_DIR "/Gameplay.ttf"
#define ICON_PATH IMAGES_DIR "/icon.png"

#endif