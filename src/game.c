#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include <time.h>
#include "game.h"
#include "input.h"
#include "graphics.h"
#include "projectile.h"
#include "enemy.h"

void initialize_game(GameState *game)
{
    SDL_Init(SDL_INIT_VIDEO);
    TTF_Init();

    // Cria janela não redimensionável
    game->window = SDL_CreateWindow("Starfall 2D",
                                    SDL_WINDOWPOS_CENTERED,
                                    SDL_WINDOWPOS_CENTERED,
                                    SCREEN_WIDTH,
                                    SCREEN_HEIGHT,
                                    0); // Flag 0 = janela fixa
    SDL_RenderSetLogicalSize(game->renderer, SCREEN_WIDTH, SCREEN_HEIGHT);

    char *base_path = SDL_GetBasePath();
    if (!base_path)
    {
        printf("Erro ao obter caminho base: %s\n", SDL_GetError());
        exit(1);
    }

    // Carrega ícone
    char full_icon_path[1024];
    snprintf(full_icon_path, sizeof(full_icon_path), "%s%s", base_path, ICON_PATH);

    SDL_Surface *icon = IMG_Load(full_icon_path);
    if (!icon)
    {
        printf("Erro ao carregar ícone em:\n%s\nErro: %s\n",
               full_icon_path, IMG_GetError());
        SDL_free(base_path);
        exit(1);
    }
    SDL_SetWindowIcon(game->window, icon);
    SDL_FreeSurface(icon);

    game->renderer = SDL_CreateRenderer(game->window, -1,
                                        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    // Carrega fonte
    char full_font_path[1024];
    snprintf(full_font_path, sizeof(full_font_path), "%s%s", base_path, FONT_MAIN);

    game->font = TTF_OpenFont(full_font_path, FONT_SIZE);
    SDL_free(base_path); // Libera memória após usar

    if (!game->font)
    {
        printf("Erro ao carregar fonte em:\n%s\nErro: %s\n",
               full_font_path, TTF_GetError());
        exit(1);
    }

    game->is_fullscreen = false;
    game->score = 0;
    game->running = true;

    // Inicialização do jogador
    init_player(&game->player); // ✅ Correto

    // Inicializa arrays
    for (int i = 0; i < MAX_PROJECTILES; i++)
    {
        game->projectiles[i].active = false;
    }
    for (int i = 0; i < MAX_ENEMIES; i++)
    {
        game->enemies[i].active = false;
    }

    srand(time(NULL));
    SDL_RenderSetLogicalSize(game->renderer, SCREEN_WIDTH, SCREEN_HEIGHT);
}
void run_game_loop(GameState *game)
{
    while (game->running)
    {
        handle_input(game);
        update_player(&game->player);
        update_projectiles(game);
        spawn_enemy(game);
        update_enemies(game);
        check_collisions(game);
        render_game(game);
        SDL_Delay(16);
    }
}

void cleanup_game(GameState *game)
{
    SDL_DestroyRenderer(game->renderer);
    SDL_DestroyWindow(game->window);
    TTF_CloseFont(game->font);
    TTF_Quit();
    SDL_Quit();
}