/**
 * @file input.c
 * @brief Implements the input handling system.
 *
 * This file contains the logic for polling SDL events and translating them
 * into a queryable state struct for use by the rest of the application.
 */

#include "core/input.h"

void input_init(InputState* input) {
  // Get a pointer to SDL's internal keyboard state array. This array is managed
  // by SDL and provides real-time key states.
  input->keyboard_state = SDL_GetKeyboardState(NULL);
  input->quit_requested = false;
  // All other boolean flags are reset per-frame in input_poll, so they do not
  // need initialization here.
}

void input_poll(InputState* input) {
  // Reset all single-frame event flags at the beginning of each frame.
  // This ensures that an event from one frame isn't accidentally processed
  // in the next.
  input->quit_requested = false;
  input->f11_pressed = false;
  input->up_pressed = false;
  input->down_pressed = false;
  input->enter_pressed = false;
  input->space_pressed = false;
  input->mouse_clicked = false;
  input->right_mouse_clicked = false;

  // Always get the latest mouse position for aiming and UI interaction.
  SDL_GetMouseState(&input->mouse_x, &input->mouse_y);

  SDL_Event event;
  // Process all events in the queue for this frame.
  while (SDL_PollEvent(&event)) {
    switch (event.type) {
      case SDL_QUIT:
        input->quit_requested = true;
        break;

      case SDL_KEYDOWN:
        // We only care about non-repeat events for single-press actions like
        // menu navigation to avoid multiple inputs from one key hold.
        if (event.key.repeat == 0) {
          switch (event.key.keysym.sym) {
            case SDLK_F11:
              input->f11_pressed = true;
              break;
            case SDLK_UP:
              input->up_pressed = true;
              break;
            case SDLK_DOWN:
              input->down_pressed = true;
              break;
            case SDLK_RETURN:
            case SDLK_KP_ENTER:  // Also check the numpad enter key.
              input->enter_pressed = true;
              break;
            case SDLK_SPACE:
              input->space_pressed = true;
              break;
          }
        }
        break;

      case SDL_MOUSEBUTTONDOWN:
        if (event.button.button == SDL_BUTTON_LEFT) {
          input->mouse_clicked = true;
        }
        if (event.button.button == SDL_BUTTON_RIGHT) {
          input->right_mouse_clicked = true;
        }
        break;
    }
  }
}
