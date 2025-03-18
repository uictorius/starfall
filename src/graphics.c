#include <stdio.h>
#include <SDL2/SDL_image.h>

#include "../include/graphics.h"
#include "../include/config.h"
#include "../include/game.h"

void render_projectiles(GameState *game) {
    for (int i = 0; i < MAX_PROJECTILES; i++) {
        if (game->projectiles[i].active) {
            // Define a cor específica para cada tipo de projétil
            SDL_Color color = game->projectiles[i].is_enemy ? 
                game->projectiles[i].color : 
                (SDL_Color){255, 255, 0, 255}; // Amarelo para jogador

            SDL_SetRenderDrawColor(
                game->renderer,
                color.r,
                color.g,
                color.b,
                color.a
            );

            SDL_Rect proj_rect = {
                game->projectiles[i].x - game->projectiles[i].radius,
                game->projectiles[i].y - game->projectiles[i].radius,
                game->projectiles[i].radius * 2,
                game->projectiles[i].radius * 2
            };
            
            SDL_RenderFillRect(game->renderer, &proj_rect);
        }
    }
}

void render_game(GameState *game)
{
    SDL_RenderClear(game->renderer); // Limpa o buffer

    switch (game->current_state)
    {
    case GAME_STATE_PLAYING:
        render_game_objects(game); // Jogo
        render_hud(game);          // HUD sobreposto
        break;

    case GAME_STATE_MENU:
        render_menu(game); // Menu principal
        break;

    case GAME_STATE_GAME_OVER:
        render_game_objects(game); // Jogo primeiro
        render_game_over(game);    // Overlay depois
        break;
    }

    SDL_RenderPresent(game->renderer); // Única apresentação por frame
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
void load_background(GameState *game)
{
    // Carrega a textura do background
    SDL_Surface *surface = IMG_Load("assets/images/background.jpg");
    if (!surface)
    {
        printf("Erro ao carregar background: %s\n", IMG_GetError());
        return;
    }

    game->bg_width = surface->w;
    game->bg_height = surface->h;
    game->background_texture = SDL_CreateTextureFromSurface(game->renderer, surface);
    SDL_FreeSurface(surface);

    if (!game->background_texture)
    {
        printf("Erro ao criar textura do background: %s\n", SDL_GetError());
    }
}

// graphics.c
void render_menu(GameState *game)
{
    // Fundo
    SDL_RenderCopy(game->renderer, game->menu_texture, NULL, NULL);

    // Opções do menu
    SDL_Color color = {255, 255, 255, 255};
    SDL_Color selected_color = {0, 255, 0, 255};

    // Título
    SDL_Surface *title_surface = TTF_RenderText_Blended(game->font, "Starfall 2D", color);
    SDL_Texture *title_texture = SDL_CreateTextureFromSurface(game->renderer, title_surface);
    SDL_Rect title_rect = {(game->current_width - title_surface->w) / 2, 100, title_surface->w, title_surface->h};
    SDL_RenderCopy(game->renderer, title_texture, NULL, &title_rect);

    // Opções
    const char *options[] = {"Jogar", "Sair"};
    int y_pos = 300;

    for (int i = 0; i < 2; i++)
    {
        SDL_Color text_color = (i == game->selected_menu_option) ? selected_color : color;

        SDL_Surface *surface = TTF_RenderText_Blended(game->font, options[i], text_color);
        SDL_Texture *texture = SDL_CreateTextureFromSurface(game->renderer, surface);
        SDL_Rect rect = {
            (game->current_width - surface->w) / 2,
            y_pos,
            surface->w,
            surface->h};

        SDL_RenderCopy(game->renderer, texture, NULL, &rect);
        y_pos += 100;

        SDL_FreeSurface(surface);
        SDL_DestroyTexture(texture);
    }

    SDL_FreeSurface(title_surface);
    SDL_DestroyTexture(title_texture);
}

void render_game_over(GameState *game)
{
    // Fundo escuro semi-transparente
    SDL_SetRenderDrawBlendMode(game->renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(game->renderer, 0, 0, 0, 200);
    SDL_Rect overlay = {0, 0, game->current_width, game->current_height};
    SDL_RenderFillRect(game->renderer, &overlay);

    // Texto
    SDL_Color color = {255, 255, 255, 255};

    char text[50];
    snprintf(text, sizeof(text), "Pontos: %d", game->score);

    SDL_Surface *surface = TTF_RenderText_Blended(game->font, "Game Over", color);
    SDL_Texture *texture = SDL_CreateTextureFromSurface(game->renderer, surface);
    SDL_Rect rect = {
        (game->current_width - surface->w) / 2,
        200,
        surface->w,
        surface->h};
    SDL_RenderCopy(game->renderer, texture, NULL, &rect);

    SDL_Surface *score_surface = TTF_RenderText_Blended(game->font, text, color);
    SDL_Texture *score_texture = SDL_CreateTextureFromSurface(game->renderer, score_surface);
    SDL_Rect score_rect = {
        (game->current_width - score_surface->w) / 2,
        300,
        score_surface->w,
        score_surface->h};
    SDL_RenderCopy(game->renderer, score_texture, NULL, &score_rect);

    // Opções
    const char *options[] = {"Reiniciar", "Menu Principal", "Sair"};
    int y_pos = 400;

    for (int i = 0; i < 3; i++)
    {
        SDL_Color text_color = (i == game->selected_menu_option) ? color : (SDL_Color){150, 150, 150, 255};

        SDL_Surface *option_surface = TTF_RenderText_Blended(game->font, options[i], text_color);
        SDL_Texture *option_texture = SDL_CreateTextureFromSurface(game->renderer, option_surface);
        SDL_Rect option_rect = {
            (game->current_width - option_surface->w) / 2,
            y_pos,
            option_surface->w,
            option_surface->h};

        SDL_RenderCopy(game->renderer, option_texture, NULL, &option_rect);
        y_pos += 70;

        SDL_FreeSurface(option_surface);
        SDL_DestroyTexture(option_texture);
    }

    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
    SDL_FreeSurface(score_surface);
    SDL_DestroyTexture(score_texture);
}

// Adicione esta função em graphics.c
void render_game_objects(GameState *game)
{
    // Renderiza o background repetido (mesmo código que está no render_game)
    if (game->background_texture)
    {
        SDL_Rect dest = {0, 0, game->bg_width, game->bg_height};
        int x_repeat = (game->current_width / game->bg_width) + 1;
        int y_repeat = (game->current_height / game->bg_height) + 1;

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

    // Jogador (mesmo código)
    SDL_SetRenderDrawColor(game->renderer, 0, 255, 0, 255);
    SDL_Rect player_rect = {
        game->player.x - game->player.radius,
        game->player.y - game->player.radius,
        game->player.radius * 2,
        game->player.radius * 2};
    SDL_RenderFillRect(game->renderer, &player_rect);

    // Projéteis
    render_projectiles(game);

    // Inimigos
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
}