#ifndef ENEMY_H
#define ENEMY_H

#include "config.h"

typedef struct GameState GameState;

typedef struct Enemy {
    float x, y;
    float dx, dy;
    int radius;
    bool active;
} Enemy;

// Declarações das funções
void spawn_enemy(GameState *game);
void update_enemies(GameState *game);
void check_collisions(GameState *game);

#endif