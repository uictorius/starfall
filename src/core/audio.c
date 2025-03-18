/**
 * @file audio.c
 * @brief Implements the functionality for the audio subsystem.
 *
 * This file contains the logic for initializing SDL_mixer, loading audio
 * assets, and controlling playback of sounds and music.
 */

#include "core/audio.h"

#include <stdio.h>

// --- Private Helpers ---

/**
 * @brief Loads a sound chunk from a file and reports any errors.
 * @param path The file path to the sound asset (.wav).
 * @return A pointer to the loaded Mix_Chunk, or NULL on failure.
 */
static Mix_Chunk* load_sound(const char* path) {
  Mix_Chunk* chunk = Mix_LoadWAV(path);
  if (!chunk) {
    fprintf(stderr, "ERROR: Failed to load sound %s: %s\n", path,
            Mix_GetError());
  }
  return chunk;
}

// --- Public API Implementations ---

bool audio_init(AudioContext* audio) {
  if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
    fprintf(stderr, "ERROR: Failed to initialize SDL_mixer: %s\n",
            Mix_GetError());
    return false;
  }

  audio->background_music = Mix_LoadMUS("assets/sounds/background.mp3");
  if (!audio->background_music) {
    fprintf(stderr, "ERROR: Failed to load music: %s\n", Mix_GetError());
    // This is not a fatal error; the game can run without music.
  }

  // Load all sound effects.
  audio->laser_sound = load_sound("assets/sounds/projectiles.wav");
  audio->explosion_sound = load_sound("assets/sounds/explosion.wav");
  audio->enemy_laser_sound = load_sound("assets/sounds/enemy_laser_sound.wav");

  // Set default volumes to balance the audio mix.
  if (audio->laser_sound)
    Mix_VolumeChunk(audio->laser_sound, 40);
  if (audio->explosion_sound)
    Mix_VolumeChunk(audio->explosion_sound, 82);
  if (audio->enemy_laser_sound)
    Mix_VolumeChunk(audio->enemy_laser_sound, 62);
  Mix_VolumeMusic(40);

  return true;
}

void audio_play_music(AudioContext* audio, bool loop) {
  if (audio->background_music) {
    // A loop value of -1 tells SDL_mixer to loop indefinitely.
    Mix_PlayMusic(audio->background_music, loop ? -1 : 1);
  }
}

void audio_play_sound(AudioContext* audio, Mix_Chunk* sound) {
  // The 'audio' parameter is unused here but is kept for API consistency.
  // This allows the audio system to become more stateful in the future without
  // changing function signatures.
  (void)audio;
  if (sound) {
    // Play on the first free channel (-1) without looping (0).
    Mix_PlayChannel(-1, sound, 0);
  }
}

void audio_cleanup(AudioContext* audio) {
  // Free all loaded audio resources.
  Mix_FreeMusic(audio->background_music);
  Mix_FreeChunk(audio->laser_sound);
  Mix_FreeChunk(audio->explosion_sound);
  Mix_FreeChunk(audio->enemy_laser_sound);

  // Shut down the SDL_mixer subsystem.
  Mix_CloseAudio();
  Mix_Quit();
}
