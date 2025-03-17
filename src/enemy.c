#include <math.h>
#include <stdlib.h>

#include "../include/enemy.h"
#include "../include/game.h"

void spawn_enemy(GameState *game) {
    if(rand() % 100 < 3) {
        for(int i = 0; i < MAX_ENEMIES; i++) {
            if(!game->enemies[i].active) {
                int side = rand() % 4;
                switch(side) {
                    case 0: game->enemies[i].x = -20; game->enemies[i].y = rand() % SCREEN_HEIGHT; break;
                    case 1: game->enemies[i].x = SCREEN_WIDTH + 20; game->enemies[i].y = rand() % SCREEN_HEIGHT; break;
                    case 2: game->enemies[i].x = rand() % SCREEN_WIDTH; game->enemies[i].y = -20; break;
                    case 3: game->enemies[i].x = rand() % SCREEN_WIDTH; game->enemies[i].y = SCREEN_HEIGHT + 20; break;
                }

                float angle = atan2(game->player.y - game->enemies[i].y, 
                                  game->player.x - game->enemies[i].x);
                game->enemies[i].dx = cos(angle) * (1 + rand() % 3);
                game->enemies[i].dy = sin(angle) * (1 + rand() % 3);
                game->enemies[i].radius = 12;
                game->enemies[i].active = true;
                break;
            }
        }
    }
}

void update_enemies(GameState *game) {
    for(int i = 0; i < MAX_ENEMIES; i++) {
        if(game->enemies[i].active) {
            game->enemies[i].x += game->enemies[i].dx;
            game->enemies[i].y += game->enemies[i].dy;

            float dx = game->player.x - game->enemies[i].x;
            float dy = game->player.y - game->enemies[i].y;
            float distance = sqrt(dx*dx + dy*dy);

            if(distance < game->player.radius + game->enemies[i].radius) {
                game->player.lives--;
                game->enemies[i].active = false;
                if(game->player.lives <= 0) game->running = false;
            }
        }
    }
}

void check_collisions(GameState *game) {
    for(int i = 0; i < MAX_PROJECTILES; i++) {
        if(game->projectiles[i].active) {
            for(int j = 0; j < MAX_ENEMIES; j++) {
                if(game->enemies[j].active) {
                    float dx = game->projectiles[i].x - game->enemies[j].x;
                    float dy = game->projectiles[i].y - game->enemies[j].y;
                    float distance = sqrt(dx*dx + dy*dy);

                    if(distance < game->projectiles[i].radius + game->enemies[j].radius) {
                        game->projectiles[i].active = false;
                        game->enemies[j].active = false;
                        game->score += 10;
                    }
                }
            }
        }
    }
}