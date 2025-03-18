#ifndef ENEMY_H
#define ENEMY_H

#include <SDL2/SDL.h> 
#include <stdbool.h>
#include "config.h"

#define ENEMY_SPAWN_OFFSET 20    // Distância fora da tela onde nascem os inimigos
#define ENEMY_DESTROY_OFFSET 100 // Distância para destruir inimigos fora da tela
#define MAX_ENEMY_PROJECTILES 50
#define ENEMY_PROJECTILE_SPEED 4.0f
#define ENEMY_SHOOT_COOLDOWN_MIN 1000  // 1 segundo
#define ENEMY_SHOOT_COOLDOWN_MAX 5000  // 5 segundos
#define ENEMY_ACCURACY_MIN 0.7f
#define ENEMY_ACCURACY_MAX 0.99f
#define PROJECTILE_DESTROY_OFFSET 50  // Distância fora da tela para destruir projéteis

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

typedef struct GameState GameState;

typedef struct Enemy {
    float x, y;
    float dx, dy;
    int radius;
    bool active;
    Uint32 next_fire;       // Tempo para próximo disparo
    float fire_cooldown;    // Cooldown entre disparos (aleatório)
    float accuracy;         // Precisão do disparo (0-1)
} Enemy;

// Declarações das funções
void spawn_enemy(GameState *game);
void update_enemies(GameState *game);
void check_collisions(GameState *game);
void enemy_shoot(GameState * game, Enemy * enemy);

#endif