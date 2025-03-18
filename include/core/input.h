/**
 * @file input.h
 * @brief Defines the public interface for the input handling system.
 *
 * This module is responsible for capturing and processing all user input,
 * including keyboard, mouse, and window events.
 */

#ifndef INPUT_H
#define INPUT_H

#include "utils/types.h"

// --- Public API ---

/**
 * @brief Initializes the input state structure.
 *
 * This function prepares the input system for polling by acquiring the
 * initial keyboard state from SDL. It should be called once at startup.
 * @param input A pointer to the InputState struct to be initialized.
 */
void input_init(InputState* input);

/**
 * @brief Polls all pending SDL events and updates the InputState for the
 * current frame.
 *
 * This function should be called exactly once per frame to capture all user
 * actions, such as key presses, mouse clicks, and quit requests.
 * @param input A pointer to the InputState struct to be updated with the
 * latest events.
 */
void input_poll(InputState* input);

#endif  // INPUT_H
