/**
 * @file game.h
 * @brief Defines the main game structure and application lifecycle functions.
 *
 * This header defines the top-level Game struct, which encapsulates the
 * entire application state, and orchestrates the primary game loop.
 */

#ifndef GAME_H
#define GAME_H

#include "game/world.h"
#include "utils/types.h"

// --- Main Game Structure ---

/**
 * @struct Game
 * @brief Encapsulates the entire state of the game application.
 *
 * This struct acts as the top-level container, holding all subsystems
 * (renderer, audio, input), the game world, and overall state information
 * like the current scene and menu selections.
 */
typedef struct {
  RendererContext renderer;  ///< The rendering subsystem context.
  AudioContext audio;        ///< The audio subsystem context.
  InputState input;          ///< The current frame's input state.
  World world;               ///< The gameplay world state.
  GameStateEnum
      current_state;  ///< The current game scene (e.g., menu, playing).
  bool is_running;    ///< The main application loop condition flag.
  int menu_option;    ///< The currently selected menu option index.
} Game;

// --- Public API ---

/**
 * @brief Initializes all game subsystems and sets the initial game state.
 * @param game A pointer to the main Game struct to be initialized.
 * @return true on successful initialization of all subsystems, false otherwise.
 */
bool game_init(Game* game);

/**
 * @brief Runs the main game loop, which handles input, updates, and rendering.
 *
 * This function contains the core application loop and will not return until
 * the game is closed.
 * @param game A pointer to the initialized Game struct.
 */
void game_run(Game* game);

/**
 * @brief Cleans up all resources used by the game and shuts down SDL
 * subsystems.
 * @param game A pointer to the Game struct whose resources will be freed.
 */
void game_cleanup(Game* game);

#endif  // GAME_H
