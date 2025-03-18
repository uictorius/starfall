/**
 * @file entities.h
 * @brief Defines the data structures for all entities within the game world.
 *
 * This header contains the definitions for the player, projectiles, and
 * enemies, which form the core components of the gameplay.
 */

#ifndef ENTITIES_H
#define ENTITIES_H

#include "utils/constants.h"
#include "utils/types.h"

/**
 * @struct Player
 * @brief Represents the state of the player's ship.
 */
typedef struct {
  float x;     ///< The X coordinate of the player's center.
  float y;     ///< The Y coordinate of the player's center.
  int radius;  ///< The collision radius of the player.
  int lives;   ///< The number of remaining lives.
} Player;

/**
 * @struct Projectile
 * @brief Represents a single projectile, fired by either the player or an
 * enemy.
 */
typedef struct {
  float x;        ///< The X coordinate of the projectile's center.
  float y;        ///< The Y coordinate of the projectile's center.
  float dx;       ///< The velocity component on the X-axis.
  float dy;       ///< The velocity component on the Y-axis.
  int radius;     ///< The collision radius of the projectile.
  bool active;    ///< Flag indicating if the projectile is currently in use.
  bool is_enemy;  ///< Flag to distinguish between player and enemy projectiles.
  SDL_Color color;  ///< The render color of the projectile.
} Projectile;

/**
 * @struct Enemy
 * @brief Represents a single enemy ship.
 */
typedef struct {
  float x;                ///< The X coordinate of the enemy's center.
  float y;                ///< The Y coordinate of the enemy's center.
  float dx;               ///< The velocity component on the X-axis.
  float dy;               ///< The velocity component on the Y-axis.
  int radius;             ///< The collision radius of the enemy.
  bool active;            ///< Flag indicating if the enemy is currently in use.
  Uint32 next_fire_time;  ///< AI timer: The next SDL_GetTicks() timestamp when
                          ///< the enemy is allowed to fire.
  Uint32
      next_retarget_time;  ///< AI timer: The next SDL_GetTicks() timestamp to
                           ///< recalculate its trajectory towards the player.
} Enemy;

#endif  // ENTITIES_H
