/**
 * @file audio.h
 * @brief Defines the public interface for the audio subsystem.
 *
 * This module manages the loading and playback of music and sound effects
 * using the SDL_mixer library.
 */

#ifndef AUDIO_H
#define AUDIO_H

#include "utils/types.h"

// --- Public API ---

/**
 * @brief Initializes the SDL_mixer subsystem and loads all audio assets.
 * @param audio A pointer to the AudioContext to be initialized.
 * @return true on successful initialization, false otherwise.
 */
bool audio_init(AudioContext* audio);

/**
 * @brief Frees all loaded audio assets and shuts down the audio subsystem.
 * @param audio A pointer to the AudioContext containing the resources to be
 * freed.
 */
void audio_cleanup(AudioContext* audio);

/**
 * @brief Plays the background music track.
 * @param audio A pointer to the AudioContext containing the music resource.
 * @param loop If true, the music will loop indefinitely; otherwise, it plays
 * once.
 */
void audio_play_music(AudioContext* audio, bool loop);

/**
 * @brief Plays a sound effect on the first available channel.
 * @param audio A pointer to the AudioContext. This is currently unused but
 * is reserved for potential future stateful audio management.
 * @param sound A pointer to the Mix_Chunk sound effect to be played.
 */
void audio_play_sound(AudioContext* audio, Mix_Chunk* sound);

#endif  // AUDIO_H
