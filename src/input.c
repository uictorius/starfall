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
    float logicalX, logicalY;
    SDL_RenderWindowToLogical(game->renderer, mouseX, mouseY, &logicalX, &logicalY);

    // Obter dimensões reais dos textos
    const char *options[] = {"Reiniciar", "Menu Principal", "Sair"};
    int option_heights[3];
    int total_height = 0;

    // Pré-calcular alturas
    for (int i = 0; i < 3; i++)
    {
        SDL_Surface *surface = TTF_RenderText_Blended(game->font, options[i], (SDL_Color){255, 255, 255, 255});
        option_heights[i] = surface->h;
        SDL_FreeSurface(surface);
        total_height += option_heights[i] + 20; // 20px de espaçamento
    }

    // Calcular posição inicial baseada no centro
    int start_y = game->current_height * 0.4f;
    int current_y = start_y;

    // Verificar colisão para cada opção
    for (int i = 0; i < 3; i++)
    {
        SDL_Surface *surface = TTF_RenderText_Blended(game->font, options[i], (SDL_Color){255, 255, 255, 255});
        int text_width = surface->w;
        SDL_FreeSurface(surface);

        if (logicalX >= (game->current_width - text_width) / 2 &&
            logicalX <= (game->current_width + text_width) / 2 &&
            logicalY >= current_y &&
            logicalY <= current_y + option_heights[i])
        {

            game->selected_menu_option = i;
            handle_game_over_selection(game);
            break;
        }
        current_y += option_heights[i] + 20;
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
    case SDL_MOUSEBUTTONDOWN:
    {
        int mouseX = event->button.x;
        int mouseY = event->button.y;
        float logicalX, logicalY;
        SDL_RenderWindowToLogical(game->renderer, mouseX, mouseY, &logicalX, &logicalY);

        const char *options[] = {"Jogar", "Sair"};
        const int option_spacing = 50; // Deve corresponder ao valor usado na renderização
        int y_pos = game->current_height * 0.4f;

        for (int i = 0; i < 2; i++)
        {
            SDL_Surface *surface = TTF_RenderText_Blended(game->font, options[i], (SDL_Color){255, 255, 255, 255});
            int text_width = surface->w;
            int text_height = surface->h;
            SDL_FreeSurface(surface);

            // Área clicável ajustada com margem vertical
            SDL_FRect click_area = {
                (game->current_width - text_width) / 2.0f,
                y_pos - 5, // Margem superior
                text_width,
                text_height + 10 // Margem inferior
            };

            if (logicalX >= click_area.x &&
                logicalX <= click_area.x + click_area.w &&
                logicalY >= click_area.y &&
                logicalY <= click_area.y + click_area.h)
            {
                if (i == 0)
                {
                    game->current_state = GAME_STATE_PLAYING;
                    reset_game(game);
                }
                else
                {
                    game->running = false;
                }
                break;
            }
            y_pos += text_height + option_spacing;
        }
        break;
    }
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