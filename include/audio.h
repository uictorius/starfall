// audio.h
#ifndef AUDIO_H
#define AUDIO_H

#include <SDL2/SDL_mixer.h>

// Estruturas para armazenar sons e músicas
typedef struct
{
    Mix_Chunk *laser_sound;
} SoundEffects;

typedef struct
{
    Mix_Music *background_music;
} Music;

int init_audio();
void load_sounds(SoundEffects *sounds);
void load_music(Music *music);
void play_laser_sound(SoundEffects *sounds);
void play_background_music(Music *music);
void cleanup_audio(SoundEffects *sounds, Music *music);

#endif // AUDIO_H
