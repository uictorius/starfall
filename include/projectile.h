#ifndef PROJECTILE_H
#define PROJECTILE_H

#include "config.h"

typedef struct GameState GameState;

typedef struct Projectile {
    float x, y;
    float dx, dy;
    int radius;
    int bounces;
    bool active;
} Projectile;

// Declarações das funções
void fire_projectile(GameState *game, int mouseX, int mouseY);
void update_projectiles(GameState *game);

#endif