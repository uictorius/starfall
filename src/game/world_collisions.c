/**
 * @file world_collisions.c
 * @brief Implements the collision detection and resolution logic for the game.
 *
 * This file is dedicated to checking for overlaps between different types of
 * game entities and applying the appropriate game rules, such as losing lives
 * or gaining score.
 */

#include <math.h>

#include "core/audio.h"
#include "game/world.h"

// --- Private Helper ---

/**
 * @brief Checks for collision between two circles using their positions and
 * radii.
 *
 * This function uses squared distances for a significant performance
 * optimization. Calculating a square root (`sqrtf`) is computationally
 * expensive, so by comparing the squared distance to the squared sum of
 * radii, we get the same result without the costly operation.
 *
 * @param x1 The X coordinate of the first circle's center.
 * @param y1 The Y coordinate of the first circle's center.
 * @param r1 The radius of the first circle.
 * @param x2 The X coordinate of the second circle's center.
 * @param y2 The Y coordinate of the second circle's center.
 * @param r2 The radius of the second circle.
 * @return True if the circles are overlapping, false otherwise.
 */
static bool check_circle_collision(float x1, float y1, int r1, float x2,
                                   float y2, int r2) {
  float dx = x1 - x2;
  float dy = y1 - y2;
  float distance_squared = dx * dx + dy * dy;
  float radii_sum = r1 + r2;
  float radii_sum_squared = radii_sum * radii_sum;
  return distance_squared < radii_sum_squared;
}

// --- Public API Implementation ---

void world_check_collisions(World* world, AudioContext* audio,
                            GameStateEnum* current_state) {
  // Iterate through all active enemies to check for collisions.
  for (int i = 0; i < MAX_ENEMIES; i++) {
    if (!world->enemies[i].active)
      continue;
    Enemy* enemy = &world->enemies[i];

    // --- 1. Enemy vs. Player Collision ---
    if (check_circle_collision(enemy->x, enemy->y, enemy->radius,
                               world->player.x, world->player.y,
                               world->player.radius)) {
      world->player.lives--;
      enemy->active = false;  // Destroy the enemy on collision.
      audio_play_sound(audio, audio->explosion_sound);
      continue;  // Skip further checks for this now-destroyed enemy.
    }

    // --- 2. Enemy vs. Player Projectiles Collision ---
    for (int j = 0; j < MAX_PROJECTILES; j++) {
      // Only check active projectiles that are not fired by enemies.
      if (!world->projectiles[j].active || world->projectiles[j].is_enemy)
        continue;
      Projectile* proj = &world->projectiles[j];

      if (check_circle_collision(enemy->x, enemy->y, enemy->radius, proj->x,
                                 proj->y, proj->radius)) {
        enemy->active = false;
        proj->active = false;
        world->score += 10;
        audio_play_sound(audio, audio->explosion_sound);
        break;  // Stop checking projectiles for this destroyed enemy.
      }
    }
  }

  // --- 3. Player vs. Enemy Projectiles Collision ---
  for (int i = 0; i < MAX_PROJECTILES; i++) {
    // Only check active projectiles that were fired by enemies.
    if (!world->projectiles[i].active || !world->projectiles[i].is_enemy)
      continue;
    Projectile* proj = &world->projectiles[i];

    if (check_circle_collision(proj->x, proj->y, proj->radius, world->player.x,
                               world->player.y, world->player.radius)) {
      world->player.lives--;
      proj->active = false;  // Destroy the projectile on collision.
      audio_play_sound(audio, audio->explosion_sound);
    }
  }

  // --- 4. Check for Game Over Condition ---
  if (world->player.lives <= 0) {
    *current_state = GAME_STATE_GAME_OVER;
  }
}
