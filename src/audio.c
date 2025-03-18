// audio.c
#include "audio.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <stdio.h>

int init_audio()
{
    if (Mix_Init(MIX_INIT_MP3 | MIX_INIT_OGG) == 0)
    {
        printf("Erro ao inicializar SDL_mixer: %s\n", Mix_GetError());
        return 0;
    }

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
    {
        printf("Erro ao abrir o áudio: %s\n", Mix_GetError());
        return 0;
    }

    return 1;
}

void load_sounds(SoundEffects *sounds)
{
    sounds->laser_sound = Mix_LoadWAV("assets/sounds/projectiles.wav");
    if (!sounds->laser_sound)
    {
        printf("Erro ao carregar som de laser: %s\n", Mix_GetError());
        exit(1);
    }
}

void load_music(Music *music)
{
    music->background_music = Mix_LoadMUS("assets/sounds/background.mp3");
    if (!music->background_music)
    {
        printf("Erro ao carregar música de fundo: %s\n", Mix_GetError());
        exit(1);
    }
}

void play_laser_sound(SoundEffects *sounds)
{
    Mix_PlayChannel(-1, sounds->laser_sound, 0); // Tocar som de laser
}

void play_background_music(Music *music)
{
    Mix_PlayMusic(music->background_music, -1); // Música em loop infinito
}

void cleanup_audio(SoundEffects *sounds, Music *music)
{
    Mix_FreeChunk(sounds->laser_sound);
    Mix_FreeMusic(music->background_music);
    Mix_CloseAudio();
    Mix_Quit();
}
