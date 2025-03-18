#include <math.h>

#include "projectile.h"
#include "game.h"

void fire_projectile(GameState *game, int mouseX, int mouseY)
{
    for (int i = 0; i < MAX_PROJECTILES; i++)
    {
        if (!game->projectiles[i].active)
        {
            float angle = atan2(mouseY - game->player.y, mouseX - game->player.x);

            // Inicialização explícita usando designated initializers
            game->projectiles[i] = (Projectile){
                .x = game->player.x,
                .y = game->player.y,
                .dx = cos(angle) * PROJECTILE_SPEED,
                .dy = sin(angle) * PROJECTILE_SPEED,
                .radius = 4,
                .active = true,
                .is_enemy = false, // Definido explicitamente como false para projéteis do jogador
                .color = (SDL_Color){255, 255, 0, 255}
            };

            break;
        }
    }
}

void update_projectiles(GameState *game) {
    for (int i = 0; i < MAX_PROJECTILES; i++) {
        if (game->projectiles[i].active) {
            // Atualiza posição
            game->projectiles[i].x += game->projectiles[i].dx;
            game->projectiles[i].y += game->projectiles[i].dy;

            // Verifica se está fora dos limites
            if (game->projectiles[i].x < -PROJECTILE_DESTROY_OFFSET || 
                game->projectiles[i].x > game->current_width + PROJECTILE_DESTROY_OFFSET ||
                game->projectiles[i].y < -PROJECTILE_DESTROY_OFFSET || 
                game->projectiles[i].y > game->current_height + PROJECTILE_DESTROY_OFFSET) 
            {
                game->projectiles[i].active = false;
            }
        }
    }
}