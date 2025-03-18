#ifndef GAME_H
#define GAME_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdbool.h>
#include "config.h"
#include <SDL2/SDL_mixer.h>

// Constantes para inicialização
#define INITIAL_ENEMY_SPEED_MULTIPLIER 1.5f
#define FPS_TARGET 60
#define FRAME_DELAY (1000 / FPS_TARGET)

typedef struct GameState GameState;
#include "player.h"
#include "projectile.h"
#include "enemy.h"

typedef struct {
    Mix_Chunk* laser_sound;
    Mix_Chunk* explosion_sound;
    Mix_Chunk* enemy_laser_sound;
} SoundEffects;

typedef struct {
    Mix_Music* background_music;
} Music;

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
    SoundEffects sounds;
    Music music;
    float enemy_speed_multiplier;
    SDL_Texture* background_texture;
    int bg_width;
    int bg_height;
    GameStateEnum current_state;
    SDL_Texture* menu_texture;
    SDL_Texture* game_over_texture;
    int selected_menu_option;
};

// Declarações de funções
void initialize_game(GameState *game);
void run_game_loop(GameState *game);
void cleanup_game(GameState *game);
void update_projectiles(GameState *game);
void initialize_menu(GameState *game);
void reset_game(GameState *game);

// Define os paths relativos
#define ASSETS_DIR "assets"
#define FONTS_DIR ASSETS_DIR "/fonts"
#define IMAGES_DIR ASSETS_DIR "/images"
#define FONT_MAIN FONTS_DIR "/Gameplay.ttf"
#define ICON_PATH IMAGES_DIR "/icon.png"

#endif