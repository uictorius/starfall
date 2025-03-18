#include "input.h"
#include "projectile.h"
#include "graphics.h"
#include "audio.h"
#include <math.h>

void handle_game_over_selection(GameState *game)
{
    switch (game->selected_menu_option)
    {
    case 0: // Reiniciar
        game->current_state = GAME_STATE_PLAYING;
        reset_game(game);
        break;
    case 1: // Menu
        game->current_state = GAME_STATE_MENU;
        break;
    case 2: // Sair
        game->running = false;
        break;
    }
}

void handle_game_over_mouse(GameState *game, int mouseX, int mouseY)
{
    // Conversão para coordenadas lógicas (float)
    float logicalX, logicalY;
    SDL_RenderWindowToLogical(
        game->renderer,
        mouseX,
        mouseY,
        &logicalX,
        &logicalY);

    // Ajuste nas verificações usando coordenadas float
    const float option_y_start = 400.0f;
    const float option_height = 70.0f;

    if (logicalY >= option_y_start && logicalY < option_y_start + option_height)
    {
        game->selected_menu_option = 0;
        handle_game_over_selection(game);
    }
    else if (logicalY >= option_y_start + option_height &&
             logicalY < option_y_start + 2 * option_height)
    {
        game->selected_menu_option = 1;
        handle_game_over_selection(game);
    }
    else if (logicalY >= option_y_start + 2 * option_height &&
             logicalY < option_y_start + 3 * option_height)
    {
        game->selected_menu_option = 2;
        handle_game_over_selection(game);
    }
}

void handle_input(GameState *game)
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        // Eventos globais (independentes do estado do jogo)
        if (event.type == SDL_WINDOWEVENT)
        {
            switch (event.window.event)
            {
            case SDL_WINDOWEVENT_RESIZED:
            {
                SDL_GetWindowSize(game->window, &game->current_width, &game->current_height);
                if (!game->is_fullscreen)
                {
                    game->stored_window_width = game->current_width;
                    game->stored_window_height = game->current_height;
                }
                SDL_RenderSetLogicalSize(game->renderer, game->current_width, game->current_height);
                break;
            }
            }
        }

        // Evento de saída global
        if (event.type == SDL_QUIT)
        {
            game->running = false;
        }

        // Tecla F11 global
        if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_F11)
        {
            toggle_fullscreen(game);
        }

        // Encaminha eventos para o estado atual
        switch (game->current_state)
        {
        case GAME_STATE_MENU:
            handle_menu_input(game, &event);
            break;

        case GAME_STATE_PLAYING:
            handle_playing_input(game, &event);
            break;

        case GAME_STATE_GAME_OVER:
            handle_game_over_input(game, &event);
            break;
        }
    }
}

// Função para estado de jogo ativo
void handle_playing_input(GameState *game, SDL_Event *event)
{
    if (event->type == SDL_MOUSEBUTTONDOWN)
    {
        int mouseX, mouseY;
        SDL_GetMouseState(&mouseX, &mouseY);

        if (Mix_PlayChannel(-1, game->sounds.laser_sound, 0) == -1)
        {
            printf("Erro ao tocar som: %s\n", Mix_GetError());
        }

        fire_projectile(game, mouseX, mouseY);
    }
}

void handle_menu_input(GameState *game, SDL_Event *event)
{
    switch (event->type)
    {
    case SDL_KEYDOWN:
        if (event->key.keysym.sym == SDLK_DOWN)
        {
            game->selected_menu_option = (game->selected_menu_option + 1) % 2;
        }
        else if (event->key.keysym.sym == SDLK_UP)
        {
            game->selected_menu_option = (game->selected_menu_option - 1 + 2) % 2;
        }
        else if (event->key.keysym.sym == SDLK_RETURN)
        {
            if (game->selected_menu_option == 0)
            { // Jogar
                game->current_state = GAME_STATE_PLAYING;
                reset_game(game); // Função para reiniciar o estado do jogo
            }
            else
            { // Sair
                game->running = false;
            }
        }
        break;

    case SDL_MOUSEBUTTONDOWN:
        int mouseX, mouseY;
        SDL_GetMouseState(&mouseX, &mouseY);

        // Verifica clique nas opções
        if (mouseY > 300 && mouseY < 400)
        {
            if (mouseX > (game->current_width - 100) / 2 && mouseX < (game->current_width + 100) / 2)
            {
                game->current_state = GAME_STATE_PLAYING;
                reset_game(game);
            }
        }
        else if (mouseY > 400 && mouseY < 500)
        {
            game->running = false;
        }
        break;
    }
}

void handle_game_over_input(GameState *game, SDL_Event *event)
{
    switch (event->type)
    {
    case SDL_KEYDOWN:
        if (event->key.keysym.sym == SDLK_DOWN)
        {
            game->selected_menu_option = (game->selected_menu_option + 1) % 3;
        }
        else if (event->key.keysym.sym == SDLK_UP)
        {
            game->selected_menu_option = (game->selected_menu_option - 1 + 3) % 3;
        }
        else if (event->key.keysym.sym == SDLK_RETURN)
        {
            handle_game_over_selection(game);
        }
        break;

    case SDL_MOUSEBUTTONDOWN:
        int mouseX, mouseY;
        SDL_GetMouseState(&mouseX, &mouseY);
        handle_game_over_mouse(game, mouseX, mouseY);
        break;
    }
}