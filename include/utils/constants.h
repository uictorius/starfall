/**
 * @file constants.h
 * @brief Defines global, compile-time constants for the game.
 *
 * This file centralizes all "magic numbers" to make tweaking gameplay
 * parameters and engine settings easier.
 */

#ifndef CONSTANTS_H
#define CONSTANTS_H

// General Game and Window Settings
#define GAME_VERSION "1.0.0"        // Current version of the game.
#define WINDOW_TITLE "Starfall 2D"  // The title displayed on the game window.
#define LOGICAL_WIDTH 1280          // The fixed internal horizontal resolution.
#define LOGICAL_HEIGHT 720          // The fixed internal vertical resolution.
#define FPS_TARGET 60  // The target frames per second for the game loop.
#define FRAME_DELAY (1000 / FPS_TARGET)  // The delay in ms to achieve the FPS.
#define FONT_SIZE_NORMAL 24              // Default font size for UI elements.
#define FONT_SIZE_LARGE 48  // Font size for titles (e.g., "Game Over").
#define FONT_SIZE_SMALL 16  // Font size for secondary text (e.g., hints).

// Player Constants
#define PLAYER_START_LIVES 5  // The number of lives the player starts with.
#define PLAYER_RADIUS 12      // The collision radius of the player's ship.
#define PLAYER_SPEED 5.0f  // The movement speed of the player in pixels/frame.

// Projectile Constants
#define MAX_PROJECTILES 200     // The maximum number of concurrent projectiles.
#define PROJECTILE_SPEED 16.0f  // The speed of player-fired projectiles.
#define PROJECTILE_RADIUS 4     // The collision radius for projectiles.
#define ENEMY_PROJECTILE_SPEED 4.0f  // The speed of enemy-fired projectiles.
#define PROJECTILE_DESTROY_OFFSET \
  50  // Extra distance off-screen before a
      // projectile is destroyed.

// Enemy Constants
#define MAX_ENEMIES 50   // The maximum number of concurrent enemies.
#define ENEMY_RADIUS 12  // The collision radius for enemies.
#define ENEMY_SPAWN_RATE \
  0.03f  // The chance (0.0 to 1.0) to spawn an enemy each frame.
#define ENEMY_SPAWN_OFFSET \
  20  // Distance from the screen edge where enemies appear.
#define ENEMY_DESTROY_OFFSET \
  100  // Extra distance off-screen before an enemy is destroyed.
#define ENEMY_SPEED_MULTIPLIER 1.5f  // Base speed multiplier for enemies.
#define ENEMY_SHOOT_COOLDOWN_MIN \
  1500  // Minimum delay (ms) between enemy shots.
#define ENEMY_SHOOT_COOLDOWN_MAX \
  4000  // Maximum delay (ms) between enemy shots.
#define ENEMY_RETARGET_COOLDOWN_MIN 2000  // Minimum delay (ms) for re-aiming.
#define ENEMY_RETARGET_COOLDOWN_MAX 5000  // Maximum delay (ms) for re-aiming.

#endif  // CONSTANTS_H
