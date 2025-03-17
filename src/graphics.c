#include <stdio.h>

#include "../include/graphics.h"
#include "../include/config.h"
#include "../include/game.h"

void render_game(GameState *game)
{
    // Limpa com cor de fundo
    SDL_SetRenderDrawColor(game->renderer, 30, 30, 40, 255);
    SDL_RenderClear(game->renderer);
    SDL_RenderSetLogicalSize(game->renderer, SCREEN_WIDTH, SCREEN_HEIGHT);

    // Calcula viewport com aspect ratio correto
    int win_w, win_h;
    SDL_GetWindowSize(game->window, &win_w, &win_h);
    

    float target_aspect = (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT;
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
    SDL_Rect border = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
    SDL_RenderDrawRect(game->renderer, &border);

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
    snprintf(text, sizeof(text), "Vidas: %d", game->player.lives);
    SDL_Surface *surface = TTF_RenderText_Solid(game->font, text, (SDL_Color){255, 255, 255, 255});
    SDL_Texture *texture = SDL_CreateTextureFromSurface(game->renderer, surface);
    SDL_Rect dest = {SCREEN_WIDTH - surface->w - 10, 10, surface->w, surface->h};
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
    SDL_SetWindowFullscreen(game->window,
                            game->is_fullscreen ? SDL_WINDOW_FULLSCREEN_DESKTOP : 0);
}