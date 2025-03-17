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
            // Bloqueia redimensionamento
            SDL_SetWindowSize(game->window, SCREEN_WIDTH, SCREEN_HEIGHT);
        }
        {
            switch (event.window.event)
            {
            case SDL_WINDOWEVENT_RESIZED:
                int w, h;
                SDL_GetWindowSize(game->window, &w, &h);

                // Mantém aspect ratio 16:9
                float target_aspect = 16.0f / 9.0f;
                int expected_width = h * target_aspect;
                int expected_height = w / target_aspect;

                if (w != expected_width || h != expected_height)
                {
                    SDL_SetWindowSize(game->window, expected_width, h);
                }
                break;
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