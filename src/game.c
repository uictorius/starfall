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

// Função auxiliar para tratamento de erros
void handle_error(const char* message, const char* error) {
    fprintf(stderr, "ERRO: %s\nDetalhes: %s\n", message, error);
    exit(EXIT_FAILURE);
}

// Função para construir caminhos de assets
void build_asset_path(char* buffer, size_t buffer_size, const char* asset_path) {
    char* base_path = SDL_GetBasePath();
    if (!base_path) {
        handle_error("Falha ao obter caminho base", SDL_GetError());
    }
    
    snprintf(buffer, buffer_size, "%s%s", base_path, asset_path);
    SDL_free(base_path);
}

void initialize_game(GameState *game) {
    // Inicialização de subsistemas
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_EVENTS) < 0) {
        handle_error("Falha na inicialização do SDL", SDL_GetError());
    }
    
    if (TTF_Init() == -1) {
        handle_error("Falha na inicialização do SDL_ttf", TTF_GetError());
    }

    // Configuração inicial da janela
    game->current_width = DEFAULT_WIDTH;
    game->current_height = DEFAULT_HEIGHT;
    
    game->window = SDL_CreateWindow(
        "Starfall 2D",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        game->current_width,
        game->current_height,
        SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI
    );
    
    if (!game->window) {
        handle_error("Falha ao criar janela", SDL_GetError());
    }

    SDL_SetWindowMinimumSize(game->window, DEFAULT_WIDTH, DEFAULT_HEIGHT);

    // Criação do renderizador
    game->renderer = SDL_CreateRenderer(
        game->window,
        -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_TARGETTEXTURE
    );
    
    if (!game->renderer) {
        handle_error("Falha ao criar renderizador", SDL_GetError());
    }

    // Configurações do renderizador
    SDL_RenderSetLogicalSize(game->renderer, DEFAULT_WIDTH, DEFAULT_HEIGHT);
    SDL_RenderSetIntegerScale(game->renderer, SDL_TRUE);
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");

    // Carregamento de recursos
    char asset_path[1024];
    
    // Ícone da janela
    build_asset_path(asset_path, sizeof(asset_path), ICON_PATH);
    SDL_Surface* icon = IMG_Load(asset_path);
    if (icon) {
        SDL_SetWindowIcon(game->window, icon);
        SDL_FreeSurface(icon);
    }

    // Fonte principal
    build_asset_path(asset_path, sizeof(asset_path), FONT_MAIN);
    game->font = TTF_OpenFont(asset_path, FONT_SIZE);
    if (!game->font) {
        handle_error("Falha ao carregar fonte", TTF_GetError());
    }

    // Inicialização de sistemas
    initialize_menu(game);
    load_background(game);
    
    if (!init_audio()) {
        handle_error("Falha na inicialização do áudio", Mix_GetError());
    }

    // Configuração inicial do jogo
    memset(game->projectiles, 0, sizeof(Projectile) * MAX_PROJECTILES);
    memset(game->enemies, 0, sizeof(Enemy) * MAX_ENEMIES);
    
    game->is_fullscreen = false;
    game->score = 0;
    game->running = true;
    game->enemy_speed_multiplier = INITIAL_ENEMY_SPEED_MULTIPLIER;

    // Inicialização do jogador
    init_player(&game->player, game);

    // Sistema de áudio
    load_sounds(game);
    load_music(game);
    
    if (Mix_PlayMusic(game->music.background_music, -1) == -1) {
        fprintf(stderr, "Aviso: Falha ao tocar música - %s\n", Mix_GetError());
    }

    // Seed para números aleatórios
    srand((unsigned int)time(NULL));
}

void run_game_loop(GameState *game) {
    Uint32 frame_start;
    Uint32 frame_time;

    while (game->running) {
        frame_start = SDL_GetTicks();

        handle_input(game);

        if (game->current_state == GAME_STATE_PLAYING) {
            update_player(&game->player, game);
            update_projectiles(game);
            spawn_enemy(game);
            update_enemies(game);
            check_collisions(game);
        }

        render_game(game);

        // Controle de FPS
        frame_time = SDL_GetTicks() - frame_start;
        if (frame_time < FRAME_DELAY) {
            SDL_Delay(FRAME_DELAY - frame_time);
        }
    }
}

void cleanup_game(GameState *game) {
    // Liberação de recursos gráficos
    if (game->background_texture) {
        SDL_DestroyTexture(game->background_texture);
        game->background_texture = NULL;
    }
    
    if (game->menu_texture) {
        SDL_DestroyTexture(game->menu_texture);
        game->menu_texture = NULL;
    }

    // Liberação de sistemas
    cleanup_audio(game);
    
    // Liberação de recursos SDL
    if (game->renderer) {
        SDL_DestroyRenderer(game->renderer);
        game->renderer = NULL;
    }
    
    if (game->window) {
        SDL_DestroyWindow(game->window);
        game->window = NULL;
    }
    
    if (game->font) {
        TTF_CloseFont(game->font);
        game->font = NULL;
    }

    // Encerramento de subsistemas
    TTF_Quit();
    IMG_Quit();
    Mix_Quit();
    SDL_Quit();
}

void reset_game(GameState *game) {
    // Reinicialização do estado do jogo
    game->score = 0;
    game->enemy_speed_multiplier = INITIAL_ENEMY_SPEED_MULTIPLIER;
    
    // Reinicialização do jogador
    init_player(&game->player, game);

    // Limpeza de entidades
    memset(game->projectiles, 0, sizeof(Projectile) * MAX_PROJECTILES);
    memset(game->enemies, 0, sizeof(Enemy) * MAX_ENEMIES);
}

void initialize_menu(GameState *game) {
    char asset_path[1024];
    build_asset_path(asset_path, sizeof(asset_path), "assets/images/background.jpg");

    SDL_Surface* menu_surface = IMG_Load(asset_path);
    if (!menu_surface) {
        handle_error("Falha ao carregar textura do menu", IMG_GetError());
    }

    game->menu_texture = SDL_CreateTextureFromSurface(game->renderer, menu_surface);
    SDL_FreeSurface(menu_surface);
    
    if (!game->menu_texture) {
        handle_error("Falha ao criar textura do menu", SDL_GetError());
    }

    game->current_state = GAME_STATE_MENU;
    game->selected_menu_option = 0;
}