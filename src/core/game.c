/**
 * @file game.c
 * @brief Implements the core game loop and state management logic.
 *
 * This file is responsible for initializing and cleaning up all subsystems,
 * running the main application loop, and delegating input handling, updates,
 * and rendering based on the current game state.
 */

#include "core/game.h"

#include <stdio.h>
#include <time.h>

#include "core/audio.h"
#include "core/input.h"
#include "core/renderer.h"
#include "game/world.h"
#include "utils/constants.h"

// --- Private Function Prototypes ---
static void game_handle_input(Game* game);
static void game_update(Game* game);
static void game_render(Game* game);

// --- Public API Implementations ---

bool game_init(Game* game) {
  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
    fprintf(stderr, "ERROR: Failed to initialize SDL: %s\n", SDL_GetError());
    return false;
  }

  // Initialize all game subsystems.
  if (!renderer_init(&game->renderer))
    return false;
  if (!audio_init(&game->audio))
    return false;

  world_init(&game->world);
  input_init(&game->input);

  // Set initial game state.
  game->is_running = true;
  game->current_state = GAME_STATE_MENU;
  game->menu_option = 0;

  // Seed the random number generator.
  srand((unsigned int)time(NULL));
  audio_play_music(&game->audio, true);
  return true;
}

void game_run(Game* game) {
  Uint32 frame_start_time;
  int frame_duration;

  // This is the main application loop.
  while (game->is_running) {
    frame_start_time = SDL_GetTicks();

    // The three core phases of the game loop.
    game_handle_input(game);
    game_update(game);
    game_render(game);

    // This implements a fixed timestep by delaying if the frame finished early.
    // It ensures a consistent game speed regardless of hardware performance.
    frame_duration = SDL_GetTicks() - frame_start_time;
    if (frame_duration < FRAME_DELAY) {
      SDL_Delay(FRAME_DELAY - frame_duration);
    }
  }
}

void game_cleanup(Game* game) {
  renderer_cleanup(&game->renderer);
  audio_cleanup(&game->audio);
  SDL_Quit();
  printf("Game cleaned up successfully.\n");
}

// --- Private Function Implementations ---

/**
 * @brief Handles all user input for the current frame based on the game state.
 * @param game A pointer to the main Game struct.
 */
static void game_handle_input(Game* game) {
  input_poll(&game->input);

  if (game->input.quit_requested) {
    game->is_running = false;
    return;
  }

  if (game->input.f11_pressed) {
    renderer_toggle_fullscreen(&game->renderer);
  }

  // Handle input differently depending on the current game scene.
  switch (game->current_state) {
    case GAME_STATE_MENU: {
      SDL_Rect play_rect = renderer_get_text_rect(
          &game->renderer, "Play", LOGICAL_WIDTH / 2, LOGICAL_HEIGHT / 2);
      SDL_Rect exit_rect = renderer_get_text_rect(
          &game->renderer, "Exit", LOGICAL_WIDTH / 2, LOGICAL_HEIGHT / 2 + 40);

      // Mouse interaction for menu buttons.
      if (game->input.mouse_clicked) {
        SDL_FPoint logical_mouse;
        renderer_window_to_logical(&game->renderer, game->input.mouse_x,
                                   game->input.mouse_y, &logical_mouse.x,
                                   &logical_mouse.y);

        SDL_Point mouse_point = {(int)logical_mouse.x, (int)logical_mouse.y};
        if (SDL_PointInRect(&mouse_point, &play_rect))
          game->menu_option = 0;
        if (SDL_PointInRect(&mouse_point, &exit_rect))
          game->menu_option = 1;
      }

      // Keyboard navigation for menu.
      if (game->input.down_pressed)
        game->menu_option = 1;
      if (game->input.up_pressed)
        game->menu_option = 0;

      // Action on selection (Enter key or mouse click).
      if (game->input.enter_pressed || game->input.mouse_clicked) {
        if (game->menu_option == 0) {
          game->current_state = GAME_STATE_PLAYING;
          world_reset(&game->world);
        } else {
          game->is_running = false;
        }
      }
      break;
    }

    case GAME_STATE_PLAYING:
      // Player firing logic is tied to Spacebar or Right Mouse Button.
      if (game->input.space_pressed || game->input.right_mouse_clicked) {
        world_fire_player_projectile(&game->world, &game->renderer,
                                     game->input.mouse_x, game->input.mouse_y,
                                     &game->audio);
      }
      break;

    case GAME_STATE_GAME_OVER: {
      SDL_Rect restart_rect =
          renderer_get_text_rect(&game->renderer, "Restart", LOGICAL_WIDTH / 2,
                                 LOGICAL_HEIGHT / 2 + 40);
      SDL_Rect menu_rect =
          renderer_get_text_rect(&game->renderer, "Main Menu",
                                 LOGICAL_WIDTH / 2, LOGICAL_HEIGHT / 2 + 80);

      // Mouse interaction for game over options.
      if (game->input.mouse_clicked) {
        SDL_FPoint logical_mouse;
        renderer_window_to_logical(&game->renderer, game->input.mouse_x,
                                   game->input.mouse_y, &logical_mouse.x,
                                   &logical_mouse.y);

        SDL_Point mouse_point = {(int)logical_mouse.x, (int)logical_mouse.y};
        if (SDL_PointInRect(&mouse_point, &restart_rect))
          game->menu_option = 0;
        if (SDL_PointInRect(&mouse_point, &menu_rect))
          game->menu_option = 1;
      }

      // Keyboard navigation for game over options.
      if (game->input.down_pressed)
        game->menu_option = (game->menu_option + 1) % 2;
      if (game->input.up_pressed)
        game->menu_option = (game->menu_option - 1 + 2) % 2;

      // Action on selection.
      if (game->input.enter_pressed || game->input.mouse_clicked) {
        if (game->menu_option == 0) {  // Restart
          game->current_state = GAME_STATE_PLAYING;
          world_reset(&game->world);
        } else {  // Main Menu
          game->current_state = GAME_STATE_MENU;
        }
      }
      break;
    }
  }
}

/**
 * @brief Updates the game logic for the current frame.
 * @param game A pointer to the main Game struct.
 */
static void game_update(Game* game) {
  // Game logic is only updated when in the 'playing' state.
  if (game->current_state == GAME_STATE_PLAYING) {
    world_update(&game->world, &game->input, &game->audio);
    world_check_collisions(&game->world, &game->audio, &game->current_state);
  }
}

/**
 * @brief Renders the current game scene based on the game state.
 * @param game A pointer to the main Game struct.
 */
static void game_render(Game* game) {
  renderer_prepare_frame(&game->renderer);

  // Render a different scene depending on the current game state.
  switch (game->current_state) {
    case GAME_STATE_MENU:
      renderer_draw_menu(&game->renderer, game->menu_option);
      break;
    case GAME_STATE_PLAYING:
      renderer_draw_game(&game->renderer, &game->world);
      renderer_draw_hud(&game->renderer, &game->world);
      break;
    case GAME_STATE_GAME_OVER:
      // Draw the final game state in the background to show where the player
      // died.
      renderer_draw_game(&game->renderer, &game->world);
      renderer_draw_hud(&game->renderer, &game->world);
      // Draw the overlay menu on top.
      renderer_draw_game_over(&game->renderer, &game->world, game->menu_option);
      break;
  }

  renderer_present_frame(&game->renderer);
}
