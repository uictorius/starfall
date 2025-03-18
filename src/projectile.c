#include <math.h>

#include "projectile.h"
#include "game.h"

void fire_projectile(GameState *game, int mouseX, int mouseY) {
    for(int i = 0; i < MAX_PROJECTILES; i++) {
        if(!game->projectiles[i].active) {
            float angle = atan2(mouseY - game->player.y, mouseX - game->player.x);
            game->projectiles[i] = (Projectile){
                game->player.x,
                game->player.y,
                cos(angle) * PROJECTILE_SPEED,
                sin(angle) * PROJECTILE_SPEED,
                5, 0, true
            };
            break;
        }
    }
}

void update_projectiles(GameState *game) {
    for(int i = 0; i < MAX_PROJECTILES; i++) {
        if(game->projectiles[i].active) {
            game->projectiles[i].x += game->projectiles[i].dx;
            game->projectiles[i].y += game->projectiles[i].dy;

            // Verifica se o projétil saiu da tela
            if (game->projectiles[i].x < 0 || game->projectiles[i].x > SCREEN_WIDTH || 
                game->projectiles[i].y < 0 || game->projectiles[i].y > SCREEN_HEIGHT) {
                game->projectiles[i].active = false;
            }
        }
    }
}