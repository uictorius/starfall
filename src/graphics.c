#include <stdio.h>
#include <SDL2/SDL_image.h> 

#include "../include/graphics.h"
#include "../include/config.h"
#include "../include/game.h"

void render_game(GameState *game)
{

    float target_aspect = (float)game->current_width / (float)game->current_height;

    // Limpa com cor de fundo
    SDL_SetRenderDrawColor(game->renderer, 30, 30, 40, 255);
    SDL_RenderClear(game->renderer);
    SDL_RenderSetLogicalSize(game->renderer, game->current_width, game->current_height);
    SDL_RenderSetIntegerScale(game->renderer, SDL_TRUE); // Garante que a escala seja um número inteiro, evitando distorções

    // Calcula viewport com aspect ratio correto
    int win_w, win_h;
    SDL_GetWindowSize(game->window, &win_w, &win_h);
    float window_aspect = (float)win_w / (float)win_h;

    SDL_Rect viewport;
    if (window_aspect > target_aspect)
    {
        // Barras laterais
        viewport.w = win_h * target_aspect;
        viewport.h = win_h;
        viewport.x = (win_w - viewport.w) / 2;
        viewport.y = 0;
    }
    else
    {
        // Barras superior/inferior
        viewport.h = win_w / target_aspect;
        viewport.w = win_w;
        viewport.x = 0;
        viewport.y = (win_h - viewport.h) / 2;
    }

    SDL_RenderSetViewport(game->renderer, &viewport);

    // Desenhar borda
    SDL_SetRenderDrawColor(game->renderer, 255, 255, 255, 255);
    SDL_Rect border = {0, 0, game->current_width, game->current_height};
    SDL_RenderDrawRect(game->renderer, &border);

    // Renderiza o background repetido
    if (game->background_texture)
    {
        SDL_Rect dest = {0, 0, game->bg_width, game->bg_height};

        // Calcula quantas repetições são necessárias
        int x_repeat = (game->current_width / game->bg_width) + 1;
        int y_repeat = (game->current_height / game->bg_height) + 1;

        // Renderiza em tile
        for (int y = 0; y < y_repeat; y++)
        {
            for (int x = 0; x < x_repeat; x++)
            {
                dest.x = x * game->bg_width;
                dest.y = y * game->bg_height;
                SDL_RenderCopy(game->renderer, game->background_texture, NULL, &dest);
            }
        }
    }

    // Desenhar jogador
    SDL_SetRenderDrawColor(game->renderer, 0, 255, 0, 255);
    SDL_Rect player_rect = {
        game->player.x - game->player.radius,
        game->player.y - game->player.radius,
        game->player.radius * 2,
        game->player.radius * 2};
    SDL_RenderFillRect(game->renderer, &player_rect);

    // Desenhar projéteis
    SDL_SetRenderDrawColor(game->renderer, 255, 255, 0, 255);
    for (int i = 0; i < MAX_PROJECTILES; i++)
    {
        if (game->projectiles[i].active)
        {
            SDL_Rect proj_rect = {
                game->projectiles[i].x - game->projectiles[i].radius,
                game->projectiles[i].y - game->projectiles[i].radius,
                game->projectiles[i].radius * 2,
                game->projectiles[i].radius * 2};
            SDL_RenderFillRect(game->renderer, &proj_rect);
        }
    }

    // Projéteis inimigos
    for (int i = 0; i < MAX_PROJECTILES; i++)
    {
        if (game->projectiles[i].active)
        {
            if (game->projectiles[i].is_enemy)
            {
                SDL_SetRenderDrawColor(game->renderer,
                                       game->projectiles[i].color.r,
                                       game->projectiles[i].color.g,
                                       game->projectiles[i].color.b,
                                       game->projectiles[i].color.a);
            }
            else
            {
                SDL_SetRenderDrawColor(game->renderer, 255, 255, 0, 255);
            }

            SDL_Rect proj_rect = {
                game->projectiles[i].x - game->projectiles[i].radius,
                game->projectiles[i].y - game->projectiles[i].radius,
                game->projectiles[i].radius * 2,
                game->projectiles[i].radius * 2};
            SDL_RenderFillRect(game->renderer, &proj_rect);
        }
    }

    // Desenhar inimigos
    SDL_SetRenderDrawColor(game->renderer, 255, 0, 0, 255);
    for (int i = 0; i < MAX_ENEMIES; i++)
    {
        if (game->enemies[i].active)
        {
            SDL_Rect enemy_rect = {
                game->enemies[i].x - game->enemies[i].radius,
                game->enemies[i].y - game->enemies[i].radius,
                game->enemies[i].radius * 2,
                game->enemies[i].radius * 2};
            SDL_RenderFillRect(game->renderer, &enemy_rect);
        }
    }

    render_hud(game);
    SDL_RenderPresent(game->renderer);
}

void render_hud(GameState *game)
{
    char text[50];
    // int text_width, text_height;

    // Vidas
    snprintf(text, sizeof(text), "Vida: %d", game->player.lives);
    SDL_Surface *surface = TTF_RenderText_Solid(game->font, text, (SDL_Color){255, 255, 255, 255});
    SDL_Texture *texture = SDL_CreateTextureFromSurface(game->renderer, surface);
    SDL_Rect dest = {game->current_width - surface->w - 10, 10, surface->w, surface->h};
    SDL_RenderCopy(game->renderer, texture, NULL, &dest);
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);

    // Pontuação
    snprintf(text, sizeof(text), "Pontos: %d", game->score);
    surface = TTF_RenderText_Solid(game->font, text, (SDL_Color){255, 255, 255, 255});
    texture = SDL_CreateTextureFromSurface(game->renderer, surface);
    dest.y += 30;
    SDL_RenderCopy(game->renderer, texture, NULL, &dest);
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}

void toggle_fullscreen(GameState *game)
{
    game->is_fullscreen = !game->is_fullscreen;

    if (game->is_fullscreen)
    {
        // Guarda o tamanho atual ANTES de entrar em fullscreen
        SDL_GetWindowSize(game->window, &game->stored_window_width, &game->stored_window_height);
        SDL_SetWindowFullscreen(game->window, SDL_WINDOW_FULLSCREEN_DESKTOP);
    }
    else
    {
        // Restaura o tamanho guardado e sai do fullscreen
        SDL_SetWindowFullscreen(game->window, 0);
        SDL_SetWindowSize(game->window, game->stored_window_width, game->stored_window_height);
    }

    // Atualiza as dimensões atuais
    SDL_GetWindowSize(game->window, &game->current_width, &game->current_height);

    // Mantém a resolução lógica baseada no aspect ratio original
    SDL_RenderSetLogicalSize(game->renderer, DEFAULT_WIDTH, DEFAULT_HEIGHT);
}

// graphics.c
void load_background(GameState *game) {
    // Carrega a textura do background
    SDL_Surface *surface = IMG_Load("assets/images/background.jpg");
    if (!surface) {
        printf("Erro ao carregar background: %s\n", IMG_GetError());
        return;
    }
    
    game->bg_width = surface->w;
    game->bg_height = surface->h;
    game->background_texture = SDL_CreateTextureFromSurface(game->renderer, surface);
    SDL_FreeSurface(surface);
    
    if (!game->background_texture) {
        printf("Erro ao criar textura do background: %s\n", SDL_GetError());
    }
}