#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include "audio.h"
#include <time.h>
#include "game.h"
#include "input.h"
#include "graphics.h"
#include "projectile.h"
#include "enemy.h"

SoundEffects sounds;
Music music;

void initialize_game(GameState *game)
{
    // Inicializa SDL com suporte a áudio
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0)
    {
        printf("Erro ao inicializar SDL: %s\n", SDL_GetError());
        exit(1);
    }

    if (TTF_Init() == -1)
    {
        printf("Erro ao inicializar TTF: %s\n", TTF_GetError());
        exit(1);
    }

    game->current_width = DEFAULT_WIDTH;
    game->current_height = DEFAULT_HEIGHT;

    game->window = SDL_CreateWindow("Starfall 2D",
                                    SDL_WINDOWPOS_CENTERED,
                                    SDL_WINDOWPOS_CENTERED,
                                    game->current_width,
                                    game->current_height,
                                    SDL_WINDOW_RESIZABLE);
    if (!game->window)
    {
        printf("Erro ao criar janela: %s\n", SDL_GetError());
        exit(1);
    }

    SDL_SetWindowMinimumSize(game->window, DEFAULT_WIDTH, DEFAULT_HEIGHT); // Define o tamanho mínimo da janela

    // Inicializa o renderer depois de garantir que a janela foi criada
    game->renderer = SDL_CreateRenderer(game->window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!game->renderer)
    {
        printf("Erro ao criar o renderer: %s\n", SDL_GetError());
        exit(1);
    }

    SDL_RenderSetLogicalSize(game->renderer, game->current_width, game->current_height);
    SDL_RenderSetIntegerScale(game->renderer, SDL_TRUE); // Garante que a escala seja um número inteiro, evitando distorções

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
        printf("Erro ao carregar ícone em:\n%s\nErro: %s\n", full_icon_path, IMG_GetError());
        SDL_free(base_path);
        exit(1);
    }
    SDL_SetWindowIcon(game->window, icon);
    SDL_FreeSurface(icon);

    // Carrega a fonte
    char full_font_path[1024];
    snprintf(full_font_path, sizeof(full_font_path), "%s%s", base_path, FONT_MAIN);

    game->font = TTF_OpenFont(full_font_path, FONT_SIZE);
    SDL_free(base_path); // Libera memória após usar

    if (!game->font)
    {
        printf("Erro ao carregar fonte em:\n%s\nErro: %s\n", full_font_path, TTF_GetError());
        exit(1);
    }

    game->is_fullscreen = false;
    game->score = 0;
    game->running = true;

    if (!init_audio())
    {
        printf("Erro ao inicializar sistema de áudio\n");
        exit(1);
    }

    // Carrega recursos de áudio
    load_sounds(game);
    load_music(game);

    // Toca música de fundo
    if (Mix_PlayMusic(game->music.background_music, -1) == -1)
    {
        printf("Erro ao tocar música: %s\n", Mix_GetError());
    }

    // Inicialização do jogador
    init_player(&game->player, game);

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
    SDL_RenderSetLogicalSize(game->renderer, game->current_width, game->current_height);
}

void run_game_loop(GameState *game)
{
    while (game->running)
    {
        handle_input(game);
        update_player(&game->player, game);
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
    cleanup_audio(game);
    SDL_DestroyRenderer(game->renderer);
    SDL_DestroyWindow(game->window);
    TTF_CloseFont(game->font);
    TTF_Quit();
    SDL_Quit();
}
