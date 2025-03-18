#include "input.h"
#include "projectile.h"
#include "graphics.h"
#include <math.h>

void handle_input(GameState *game)
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
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

        if (event.type == SDL_QUIT)
        {
            game->running = false;
        }

        if (event.type == SDL_MOUSEBUTTONDOWN)
        {
            int mouseX, mouseY;
            SDL_GetMouseState(&mouseX, &mouseY);
            fire_projectile(game, mouseX, mouseY);
        }

        if (event.type == SDL_KEYDOWN)
        {
            if (event.key.keysym.sym == SDLK_F11)
            {
                toggle_fullscreen(game);
            }
        }
    }
}
