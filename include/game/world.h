/**
 * @file world.h
 * @brief Defines the game world structure and its management functions.
 *
 * This module defines the main World struct, which acts as a container for all
 * gameplay entities, and provides the public API for initializing, updating,
 * and managing the game state.
 */

#ifndef WORLD_H
#define WORLD_H

#include "entities.h"
#include "utils/types.h"

// --- Main World Structure ---

/**
 * @struct World
 * @brief Contains the entire gameplay state, including all entities and game
 * rules.
 *
 * This struct acts as a container for the player, pools of enemies and
 * projectiles, and gameplay-related data like the score and difficulty scaling.
 */
typedef struct {
  Player player;                            ///< The player entity.
  Projectile projectiles[MAX_PROJECTILES];  ///< Pool of all projectiles.
  Enemy enemies[MAX_ENEMIES];               ///< Pool of all enemies.
  int score;                                ///< The player's current score.
  float enemy_speed_multiplier;  ///< Current speed modifier for enemies.
} World;

// --- Public API ---

// Lifecycle
/**
 * @brief Initializes the world state for the very first time.
 * @param world A pointer to the World struct to initialize.
 */
void world_init(World* world);

/**
 * @brief Resets the world state to its initial configuration for a new game
 * session.
 * @param world A pointer to the World struct to reset.
 */
void world_reset(World* world);

// Core Logic
/**
 * @brief Updates all entities and game logic for a single frame.
 * @param world A pointer to the World struct.
 * @param input A constant pointer to the current input state for player
 * movement.
 * @param audio A pointer to the audio context for playing sounds.
 */
void world_update(World* world, const InputState* input, AudioContext* audio);

// Spawning
/**
 * @brief Creates a new player projectile originating from the player and aimed
 * at the mouse cursor.
 * @param world A pointer to the World struct.
 * @param renderer A pointer to the renderer context for coordinate conversion.
 * @param mouse_x The current window X coordinate of the mouse.
 * @param mouse_y The current window Y coordinate of the mouse.
 * @param audio A pointer to the audio context to play the firing sound.
 */
void world_fire_player_projectile(World* world, RendererContext* renderer,
                                  int mouse_x, int mouse_y,
                                  AudioContext* audio);

// Collisions
/**
 * @brief Checks and handles all collisions between game entities.
 *
 * This function orchestrates all collision checks (player vs. enemy,
 * projectiles vs. entities) and updates the game state accordingly.
 * It is implemented in `world_collisions.c`.
 * @param world A pointer to the World struct.
 * @param audio A pointer to the audio context for playing explosion sounds.
 * @param current_state A pointer to the main game state, which will be set to
 * GAME_STATE_GAME_OVER if the player's lives reach zero.
 */
void world_check_collisions(World* world, AudioContext* audio,
                            GameStateEnum* current_state);

#endif  // WORLD_H
