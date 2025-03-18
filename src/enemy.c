#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include "../include/enemy.h"
#include "../include/game.h"
#include "../include/audio.h"

void spawn_enemy(GameState *game)
{
    if (rand() % 100 < 3)
    {
        for (int i = 0; i < MAX_ENEMIES; i++)
        {
            if (!game->enemies[i].active)
            {
                // Posicionamento inicial
                int side = rand() % 4;
                switch (side)
                {
                case 0:
                    game->enemies[i].x = -ENEMY_SPAWN_OFFSET;
                    game->enemies[i].y = rand() % game->current_height;
                    break;
                case 1:
                    game->enemies[i].x = game->current_width + ENEMY_SPAWN_OFFSET;
                    game->enemies[i].y = rand() % game->current_height;
                    break;
                case 2:
                    game->enemies[i].x = rand() % game->current_width;
                    game->enemies[i].y = -ENEMY_SPAWN_OFFSET;
                    break;
                case 3:
                    game->enemies[i].x = rand() % game->current_width;
                    game->enemies[i].y = game->current_height + ENEMY_SPAWN_OFFSET;
                    break;
                }

                // Movimento
                float angle = atan2(game->player.y - game->enemies[i].y,
                                    game->player.x - game->enemies[i].x);
                float base_speed = (1 + (rand() % 3)) * game->enemy_speed_multiplier;

                game->enemies[i].dx = cos(angle) * base_speed;
                game->enemies[i].dy = sin(angle) * base_speed;

                // Propriedades de disparo
                game->enemies[i].fire_cooldown = ENEMY_SHOOT_COOLDOWN_MIN +
                                                 rand() % (ENEMY_SHOOT_COOLDOWN_MAX - ENEMY_SHOOT_COOLDOWN_MIN);
                game->enemies[i].next_fire = SDL_GetTicks() + (rand() % 2000);
                game->enemies[i].accuracy = ENEMY_ACCURACY_MIN +
                                            (rand() % (int)((ENEMY_ACCURACY_MAX - ENEMY_ACCURACY_MIN) * 100)) / 100.0f;

                // Propriedades gerais
                game->enemies[i].radius = 12;
                game->enemies[i].active = true;
                break;
            }
        }
    }
}

void update_enemies(GameState *game)
{
    Uint32 current_time = SDL_GetTicks();

    for (unsigned int i = 0; i < MAX_ENEMIES; i++)
    { // unsigned para comparação
        if (game->enemies[i].active)
        {
            // Atualização de posição
            game->enemies[i].x += game->enemies[i].dx;
            game->enemies[i].y += game->enemies[i].dy;

            // Lógica de disparo
            if (current_time > game->enemies[i].next_fire)
            {
                enemy_shoot(game, &game->enemies[i]);
                game->enemies[i].next_fire = current_time + game->enemies[i].fire_cooldown;
            }

            // Verificação de limites da tela
            if (game->enemies[i].x < -ENEMY_DESTROY_OFFSET ||
                game->enemies[i].x > game->current_width + ENEMY_DESTROY_OFFSET ||
                game->enemies[i].y < -ENEMY_DESTROY_OFFSET ||
                game->enemies[i].y > game->current_height + ENEMY_DESTROY_OFFSET)
            {
                game->enemies[i].active = false;
                continue;
            }

            // Colisão com jogador
            float dx = game->player.x - game->enemies[i].x;
            float dy = game->player.y - game->enemies[i].y;
            float distance = sqrt(dx * dx + dy * dy);

            if (distance < game->player.radius + game->enemies[i].radius)
            {
                game->player.lives--;
                game->enemies[i].active = false;
                if (game->player.lives <= 0)
                    game->running = false;
            }
        }
    }
}

void check_collisions(GameState *game)
{
    // Colisão entre projéteis do jogador e inimigos
    for (int i = 0; i < MAX_PROJECTILES; i++)
    {
        if (game->projectiles[i].active && !game->projectiles[i].is_enemy)
        {
            for (int j = 0; j < MAX_ENEMIES; j++)
            {
                if (game->enemies[j].active)
                {
                    float dx = game->projectiles[i].x - game->enemies[j].x;
                    float dy = game->projectiles[i].y - game->enemies[j].y;
                    float distance = sqrt(dx * dx + dy * dy);

                    if (distance < game->projectiles[i].radius + game->enemies[j].radius)
                    {
                        if (Mix_PlayChannel(-1, game->sounds.explosion_sound, 0) == -1)
                        {
                            printf("Erro ao tocar som: %s\n", Mix_GetError());
                        }
                        game->projectiles[i].active = false;
                        game->enemies[j].active = false;
                        game->score += 1;
                    }
                }
            }
        }
    }

    // Colisão entre projéteis inimigos e jogador
    for (int i = 0; i < MAX_PROJECTILES; i++)
    {
        if (game->projectiles[i].active && game->projectiles[i].is_enemy)
        {
            float dx = game->projectiles[i].x - game->player.x;
            float dy = game->projectiles[i].y - game->player.y;
            float distance = sqrt(dx * dx + dy * dy);

            if (distance < game->projectiles[i].radius + game->player.radius)
            {
                if (Mix_PlayChannel(-1, game->sounds.explosion_sound, 0) == -1)
                {
                    printf("Erro ao tocar som: %s\n", Mix_GetError());
                }
                game->player.lives--;
                game->projectiles[i].active = false;

                if (game->player.lives <= 0)
                {
                    game->running = false;
                }
            }
        }
    }

    // Colisão entre projéteis do jogador e inimigos
    for (int i = 0; i < MAX_PROJECTILES; i++)
    {
        if (game->projectiles[i].active && !game->projectiles[i].is_enemy)
        {
            for (int j = 0; j < MAX_PROJECTILES; j++)
            {
                if (game->projectiles[j].active && game->projectiles[j].is_enemy)
                {
                    float dx = game->projectiles[i].x - game->projectiles[j].x;
                    float dy = game->projectiles[i].y - game->projectiles[j].y;
                    float distance = sqrt(dx * dx + dy * dy);

                    if (distance < game->projectiles[i].radius + game->projectiles[j].radius)
                    {
                        if (Mix_PlayChannel(-1, game->sounds.explosion_sound, 0) == -1)
                        {
                            printf("Erro ao tocar som: %s\n", Mix_GetError());
                        }
                        game->projectiles[i].active = false;
                        game->projectiles[j].active = false;
                    }
                }
            }
        }
    }
}

void enemy_shoot(GameState *game, Enemy *enemy)
{
    // Cálculo da direção com imprecisão
    float angle = atan2(game->player.y - enemy->y,
                        game->player.x - enemy->x);

    // Cálculo do desvio
    float max_deviation = (1.0f - enemy->accuracy) * M_PI / 4;
    angle += ((rand() % 2000 - 1000) / 1000.0f) * max_deviation;

    // Encontrar projétil disponível
    for (int i = 0; i < MAX_PROJECTILES; i++)
    {
        if (!game->projectiles[i].active)
        {
            // Inicialização do projétil
            game->projectiles[i].x = enemy->x;
            game->projectiles[i].y = enemy->y;
            game->projectiles[i].dx = cos(angle) * ENEMY_PROJECTILE_SPEED;
            game->projectiles[i].dy = sin(angle) * ENEMY_PROJECTILE_SPEED;
            game->projectiles[i].radius = 4;
            game->projectiles[i].active = true;
            game->projectiles[i].is_enemy = true;
            game->projectiles[i].color = (SDL_Color){255, 50, 50, 255};

            // Som de disparo inimigo
            if (Mix_PlayChannel(-1, game->sounds.enemy_laser_sound, 0) == -1)
            {
                printf("Erro ao tocar som: %s\n", Mix_GetError());
            }
            break;
        }
    }
}