/**
 * @file world.c
 * @brief Implements the game world's logic, including entity updates and
 * spawning.
 *
 * This file contains the core gameplay logic for moving the player, updating
 * projectiles, and controlling enemy AI behavior.
 */

#include "game/world.h"

#include <math.h>
#include <stdlib.h>
#include <string.h>

#include "core/audio.h"
#include "core/renderer.h"

// Define PI if it's not available in the math library, for portability.
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// --- Private Function Prototypes ---
static void update_player(Player* player, const InputState* input);
static void update_projectiles(Projectile projectiles[]);
static void spawn_enemy(World* world);
static void update_enemies(World* world, AudioContext* audio);

// --- Public API Implementations ---

void world_init(World* world) {
  // For the initial setup, a full reset is sufficient.
  world_reset(world);
}

void world_reset(World* world) {
  // Use memset to efficiently zero out the entire world structure, deactivating
  // all entities.
  memset(world, 0, sizeof(World));

  // Set up the initial state for the player.
  world->player.x = LOGICAL_WIDTH / 2.0f;
  world->player.y = LOGICAL_HEIGHT / 2.0f;
  world->player.radius = PLAYER_RADIUS;
  world->player.lives = PLAYER_START_LIVES;

  // Reset score and difficulty modifiers.
  world->score = 0;
  world->enemy_speed_multiplier = ENEMY_SPEED_MULTIPLIER;
}

void world_update(World* world, const InputState* input, AudioContext* audio) {
  update_player(&world->player, input);
  update_projectiles(world->projectiles);
  spawn_enemy(world);
  update_enemies(world, audio);
}

void world_fire_player_projectile(World* world, RendererContext* renderer,
                                  int mouse_x, int mouse_y,
                                  AudioContext* audio) {
  // Find the first inactive projectile in the pool to reuse.
  for (int i = 0; i < MAX_PROJECTILES; i++) {
    if (!world->projectiles[i].active) {
      Projectile* p = &world->projectiles[i];

      float logical_x, logical_y;
      renderer_window_to_logical(renderer, mouse_x, mouse_y, &logical_x,
                                 &logical_y);

      // Calculate the angle from the player to the logical mouse position.
      float angle =
          atan2f(logical_y - world->player.y, logical_x - world->player.x);
      p->x = world->player.x;
      p->y = world->player.y;
      p->dx = cosf(angle) * PROJECTILE_SPEED;
      p->dy = sinf(angle) * PROJECTILE_SPEED;
      p->radius = PROJECTILE_RADIUS;
      p->active = true;
      p->is_enemy = false;
      p->color = (SDL_Color){255, 255, 0, 255};  // Yellow for player shots.
      audio_play_sound(audio, audio->laser_sound);
      return;  // Exit after firing one projectile to prevent machine-gunning.
    }
  }
}

// --- Private Function Implementations ---

/**
 * @brief Updates the player's position based on keyboard input and clamps it
 * to the screen bounds.
 * @param player A pointer to the player struct to be updated.
 * @param input A constant pointer to the current input state.
 */
static void update_player(Player* player, const InputState* input) {
  float dx = 0.0f, dy = 0.0f;

  // Aggregate input from both WASD and arrow keys.
  if (input->keyboard_state[SDL_SCANCODE_W] ||
      input->keyboard_state[SDL_SCANCODE_UP])
    dy -= 1.0f;
  if (input->keyboard_state[SDL_SCANCODE_S] ||
      input->keyboard_state[SDL_SCANCODE_DOWN])
    dy += 1.0f;
  if (input->keyboard_state[SDL_SCANCODE_A] ||
      input->keyboard_state[SDL_SCANCODE_LEFT])
    dx -= 1.0f;
  if (input->keyboard_state[SDL_SCANCODE_D] ||
      input->keyboard_state[SDL_SCANCODE_RIGHT])
    dx += 1.0f;

  // Normalize the movement vector to ensure constant speed in all directions.
  // Without this, diagonal movement would be faster than cardinal movement.
  float len = sqrtf(dx * dx + dy * dy);
  if (len > 0.0f) {
    player->x += (dx / len) * PLAYER_SPEED;
    player->y += (dy / len) * PLAYER_SPEED;
  }

  // Clamp player position to keep the ship within the logical screen bounds.
  player->x =
      fmax(player->radius, fmin(player->x, LOGICAL_WIDTH - player->radius));
  player->y =
      fmax(player->radius, fmin(player->y, LOGICAL_HEIGHT - player->radius));
}

/**
 * @brief Updates the positions of all active projectiles and deactivates them
 * if they go off-screen.
 * @param projectiles An array containing the projectile pool.
 */
static void update_projectiles(Projectile projectiles[]) {
  for (int i = 0; i < MAX_PROJECTILES; i++) {
    if (projectiles[i].active) {
      projectiles[i].x += projectiles[i].dx;
      projectiles[i].y += projectiles[i].dy;

      // Deactivate the projectile if it's far off-screen to free it up for
      // reuse.
      if (projectiles[i].x < -PROJECTILE_DESTROY_OFFSET ||
          projectiles[i].x > LOGICAL_WIDTH + PROJECTILE_DESTROY_OFFSET ||
          projectiles[i].y < -PROJECTILE_DESTROY_OFFSET ||
          projectiles[i].y > LOGICAL_HEIGHT + PROJECTILE_DESTROY_OFFSET) {
        projectiles[i].active = false;
      }
    }
  }
}

/**
 * @brief Potentially spawns a new enemy at a random position just outside the
 * screen bounds.
 * @param world A pointer to the game world.
 */
static void spawn_enemy(World* world) {
  // Use a random chance to determine if an enemy should spawn this frame.
  // This prevents enemies from spawning every single frame.
  if ((float)rand() / RAND_MAX > ENEMY_SPAWN_RATE)
    return;

  // Find an inactive enemy slot to use.
  for (int i = 0; i < MAX_ENEMIES; i++) {
    if (!world->enemies[i].active) {
      Enemy* enemy = &world->enemies[i];
      int side = rand() % 4;
      // Determine spawn position based on a randomly chosen screen edge.
      switch (side) {
        case 0:  // Left
          enemy->x = -ENEMY_SPAWN_OFFSET;
          enemy->y = rand() % LOGICAL_HEIGHT;
          break;
        case 1:  // Right
          enemy->x = LOGICAL_WIDTH + ENEMY_SPAWN_OFFSET;
          enemy->y = rand() % LOGICAL_HEIGHT;
          break;
        case 2:  // Top
          enemy->x = rand() % LOGICAL_WIDTH;
          enemy->y = -ENEMY_SPAWN_OFFSET;
          break;
        case 3:  // Bottom
          enemy->x = rand() % LOGICAL_WIDTH;
          enemy->y = LOGICAL_HEIGHT + ENEMY_SPAWN_OFFSET;
          break;
      }

      // Initial velocity is aimed at the player's current position.
      float angle =
          atan2f(world->player.y - enemy->y, world->player.x - enemy->x);
      enemy->dx = cosf(angle) * world->enemy_speed_multiplier;
      enemy->dy = sinf(angle) * world->enemy_speed_multiplier;
      enemy->radius = ENEMY_RADIUS;
      enemy->active = true;

      // Set initial timers for AI behaviors (firing and retargeting).
      Uint32 current_time = SDL_GetTicks();
      enemy->next_fire_time =
          current_time + ENEMY_SHOOT_COOLDOWN_MIN +
          (rand() % (ENEMY_SHOOT_COOLDOWN_MAX - ENEMY_SHOOT_COOLDOWN_MIN));
      enemy->next_retarget_time = current_time + ENEMY_RETARGET_COOLDOWN_MIN +
                                  (rand() % (ENEMY_RETARGET_COOLDOWN_MAX -
                                             ENEMY_RETARGET_COOLDOWN_MIN));
      return;  // Exit after spawning one enemy per frame check.
    }
  }
}

/**
 * @brief Updates enemy positions, handles their shooting logic, and re-targets
 * the player periodically.
 * @param world A pointer to the game world.
 * @param audio A pointer to the audio context for playing enemy firing sounds.
 */
static void update_enemies(World* world, AudioContext* audio) {
  Uint32 current_time = SDL_GetTicks();
  for (int i = 0; i < MAX_ENEMIES; i++) {
    if (world->enemies[i].active) {
      Enemy* e = &world->enemies[i];

      // AI Retargeting logic: Periodically update the enemy's trajectory to
      // follow the player.
      if (current_time > e->next_retarget_time) {
        float angle = atan2f(world->player.y - e->y, world->player.x - e->x);
        e->dx = cosf(angle) * world->enemy_speed_multiplier;
        e->dy = sinf(angle) * world->enemy_speed_multiplier;
        // Set the timer for the next retarget.
        e->next_retarget_time = current_time + ENEMY_RETARGET_COOLDOWN_MIN +
                                (rand() % (ENEMY_RETARGET_COOLDOWN_MAX -
                                           ENEMY_RETARGET_COOLDOWN_MIN));
      }

      // Update position based on current velocity.
      e->x += e->dx;
      e->y += e->dy;

      // AI Firing logic: Fire a projectile if the cooldown has elapsed.
      if (current_time > e->next_fire_time) {
        // Find an inactive projectile to fire.
        for (int j = 0; j < MAX_PROJECTILES; j++) {
          if (!world->projectiles[j].active) {
            Projectile* p = &world->projectiles[j];
            float angle =
                atan2f(world->player.y - e->y, world->player.x - e->x);
            p->x = e->x;
            p->y = e->y;
            p->dx = cosf(angle) * ENEMY_PROJECTILE_SPEED;
            p->dy = sinf(angle) * ENEMY_PROJECTILE_SPEED;
            p->radius = PROJECTILE_RADIUS;
            p->active = true;
            p->is_enemy = true;
            p->color = (SDL_Color){255, 50, 50, 255};  // Red for enemy shots.
            audio_play_sound(audio, audio->enemy_laser_sound);
            break;  // Only fire one projectile per cooldown cycle.
          }
        }
        // Reset the firing cooldown timer.
        e->next_fire_time =
            current_time + ENEMY_SHOOT_COOLDOWN_MIN +
            (rand() % (ENEMY_SHOOT_COOLDOWN_MAX - ENEMY_SHOOT_COOLDOWN_MIN));
      }
    }
  }
}
