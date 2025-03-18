/**
 * @file main.c
 * @brief The main entry point for the Starfall 2D application.
 */

#include "core/game.h"

/**
 * @brief The main function, serving as the application's entry point.
 * @param argc The number of command-line arguments.
 * @param argv An array of command-line argument strings.
 * @return 0 on successful execution, 1 on initialization failure.
 */
int main(int argc, char* argv[]) {
  // Mark arguments as unused to prevent compiler warnings.
  (void)argc;
  (void)argv;

  Game game;

  // Initialize all game systems.
  if (!game_init(&game)) {
    // An error during initialization is fatal and the program should exit.
    return 1;
  }

  // Run the main game loop. This function will block until the game is quit.
  game_run(&game);

  // Clean up all allocated resources before exiting.
  game_cleanup(&game);

  return 0;
}
