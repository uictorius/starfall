/**
 * @file types.h
 * @brief Defines core data structures, enums, and type definitions used
 * across the entire application.
 */

#ifndef TYPES_H
#define TYPES_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>
#include <stdbool.h>

/**
 * @enum GameStateEnum
 * @brief Represents the main states or scenes of the game application.
 */
typedef enum {
  GAME_STATE_MENU,      ///< The main menu is active.
  GAME_STATE_PLAYING,   ///< The main gameplay is active.
  GAME_STATE_GAME_OVER  ///< The game over screen is active.
} GameStateEnum;

/**
 * @struct RendererContext
 * @brief Holds all SDL-related rendering resources and state.
 */
typedef struct {
  SDL_Window* window;               ///< The main game window.
  SDL_Renderer* renderer;           ///< The primary SDL renderer.
  TTF_Font* font_normal;            ///< The normal font for UI text.
  TTF_Font* font_large;             ///< The large font for titles.
  TTF_Font* font_small;             ///< The small font for secondary text.
  SDL_Texture* background_texture;  ///< Cached background texture.
  SDL_Texture* game_texture;  ///< The render target for the logical scene.
  SDL_Rect viewport;          ///< The calculated viewport for scaled rendering.
  bool is_fullscreen;         ///< Current fullscreen state flag.
} RendererContext;

/**
 * @struct AudioContext
 * @brief Holds all loaded SDL_mixer audio resources.
 */
typedef struct {
  Mix_Music* background_music;   ///< The main background music track.
  Mix_Chunk* laser_sound;        ///< Sound for the player's projectile.
  Mix_Chunk* explosion_sound;    ///< Sound for collisions and destructions.
  Mix_Chunk* enemy_laser_sound;  ///< Sound for the enemy's projectile.
} AudioContext;

/**
 * @struct InputState
 * @brief Captures a snapshot of all user inputs for a single frame.
 */
typedef struct {
  const Uint8* keyboard_state;  ///< Direct, real-time state of the keyboard.
  bool quit_requested;  ///< True if the user requested to close the game.
  bool f11_pressed;     ///< A single-frame flag for the F11 key press.
  bool up_pressed;      ///< A single-frame flag for the Up Arrow key press.
  bool down_pressed;    ///< A single-frame flag for the Down Arrow key press.
  bool enter_pressed;   ///< A single-frame flag for the Enter key press.
  bool space_pressed;   ///< A single-frame flag for the Spacebar press.
  bool mouse_clicked;   ///< A single-frame flag for a left mouse button click.
  bool right_mouse_clicked;  ///< A single-frame flag for a right mouse button
                             ///< click.
  int mouse_x;               ///< Current mouse X coordinate in window space.
  int mouse_y;               ///< Current mouse Y coordinate in window space.
} InputState;

#endif  // TYPES_H
