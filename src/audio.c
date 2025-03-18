#include "audio.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <stdio.h>

int init_audio()
{
    // Inicializa SDL_mixer com suporte a formatos comuns
    int flags = MIX_INIT_MP3 | MIX_INIT_OGG | MIX_INIT_FLAC;
    if ((Mix_Init(flags) & flags) != flags)
    {
        printf("Erro ao inicializar SDL_mixer: %s\n", Mix_GetError());
        return 0;
    }

    // Configuração de áudio padrão
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
    {
        printf("Erro ao abrir o áudio: %s\n", Mix_GetError());
        return 0;
    }

    // Aloca canais de áudio
    Mix_AllocateChannels(32);
    return 1;
}
void load_sounds(GameState *game)
{
    char *base_path = SDL_GetBasePath();

    // Carrega som de laser (volume mais baixo)
    char path[1024];
    snprintf(path, sizeof(path), "%sassets/sounds/projectiles.wav", base_path);
    game->sounds.laser_sound = Mix_LoadWAV(path);
    if (!game->sounds.laser_sound)
    {
        printf("Erro ao carregar som de laser: %s\n", Mix_GetError());
    }
    else
    {
        Mix_VolumeChunk(game->sounds.laser_sound, 40);
    }

    // Carrega som de explosão (volume médio)
    snprintf(path, sizeof(path), "%sassets/sounds/explosion.wav", base_path);
    game->sounds.explosion_sound = Mix_LoadWAV(path);
    if (!game->sounds.explosion_sound)
    {
        printf("Erro ao carregar som de explosão: %s\n", Mix_GetError());
    }
    else
    {
        Mix_VolumeChunk(game->sounds.explosion_sound, 82);
    }

    // Carrega som de laser do inimigo (volume baixo)
    snprintf(path, sizeof(path), "%sassets/sounds/enemy_laser_sound.wav", base_path);
    game->sounds.enemy_laser_sound = Mix_LoadWAV(path);
    if (!game->sounds.enemy_laser_sound)
    {
        printf("Erro ao carregar som de laser do inimigo: %s\n", Mix_GetError());
    }
    else
    {
        Mix_VolumeChunk(game->sounds.enemy_laser_sound, 62);
    }

    SDL_free(base_path);
}

void load_music(GameState *game)
{
    char *base_path = SDL_GetBasePath();
    char path[1024];

    snprintf(path, sizeof(path), "%sassets/sounds/background.mp3", base_path);
    game->music.background_music = Mix_LoadMUS(path);
    if (!game->music.background_music)
    {
        printf("Erro ao carregar música: %s\n", Mix_GetError());
    }
    else
    {
        Mix_VolumeMusic(40);
    }

    SDL_free(base_path);
}

void cleanup_audio(GameState *game)
{
    if (game->sounds.laser_sound)
        Mix_FreeChunk(game->sounds.laser_sound);
    if (game->sounds.explosion_sound)
        Mix_FreeChunk(game->sounds.explosion_sound);
    if (game->sounds.enemy_laser_sound)
        Mix_FreeChunk(game->sounds.enemy_laser_sound);
    if (game->music.background_music)
        Mix_FreeMusic(game->music.background_music);

    Mix_CloseAudio();
    Mix_Quit();
}