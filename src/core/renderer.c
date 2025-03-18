/**
 * @file renderer.c
 * @brief Implements the rendering subsystem functionality.
 *
 * This file contains the logic for the dynamic scaling renderer. The game is
 * drawn to an internal texture at a fixed logical resolution. This texture is
 * then scaled to fit the current window size using a stretch-to-fill method.
 */

#include "core/renderer.h"

#include <SDL2/SDL_image.h>
#include <stdio.h>

#include "utils/constants.h"

// --- Private Helper Prototypes ---
static void render_text(SDL_Renderer* renderer, TTF_Font* font,
                        const char* text, int x, int y, SDL_Color color,
                        bool centered);
static void update_viewport(RendererContext* context);

// --- Public API Implementations ---

bool renderer_init(RendererContext* context) {
  if (TTF_Init() == -1) {
    fprintf(stderr, "ERROR: Failed to initialize SDL_ttf: %s\n",
            TTF_GetError());
    return false;
  }

  char window_title[128];
  snprintf(window_title, sizeof(window_title), "%s - v%s", WINDOW_TITLE,
           GAME_VERSION);

  context->window = SDL_CreateWindow(window_title, SDL_WINDOWPOS_CENTERED,
                                     SDL_WINDOWPOS_CENTERED, LOGICAL_WIDTH,
                                     LOGICAL_HEIGHT, SDL_WINDOW_RESIZABLE);
  if (!context->window) {
    fprintf(stderr, "ERROR: Failed to create window: %s\n", SDL_GetError());
    return false;
  }

  context->renderer =
      SDL_CreateRenderer(context->window, -1,
                         SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
  if (!context->renderer) {
    fprintf(stderr, "ERROR: Failed to create renderer: %s\n", SDL_GetError());
    return false;
  }

  // Create the main game texture. All gameplay rendering will be done to this
  // texture at a fixed logical resolution.
  context->game_texture = SDL_CreateTexture(
      context->renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET,
      LOGICAL_WIDTH, LOGICAL_HEIGHT);
  if (!context->game_texture) {
    fprintf(stderr, "ERROR: Failed to create game texture: %s\n",
            SDL_GetError());
    return false;
  }

  context->is_fullscreen = false;

  // Load all required fonts.
  context->font_normal =
      TTF_OpenFont("assets/fonts/Gameplay.ttf", FONT_SIZE_NORMAL);
  context->font_large =
      TTF_OpenFont("assets/fonts/Gameplay.ttf", FONT_SIZE_LARGE);
  context->font_small =
      TTF_OpenFont("assets/fonts/Gameplay.ttf", FONT_SIZE_SMALL);
  if (!context->font_normal || !context->font_large || !context->font_small) {
    fprintf(stderr, "ERROR: Failed to load font: %s\n", TTF_GetError());
    // Fonts are critical for the UI, so this is a fatal error.
    return false;
  }

  // Load background texture.
  SDL_Surface* bg_surface = IMG_Load("assets/images/background.jpg");
  if (bg_surface) {
    context->background_texture =
        SDL_CreateTextureFromSurface(context->renderer, bg_surface);
    SDL_FreeSurface(bg_surface);
  } else {
    // A missing background is not a fatal error; the game can still run.
    fprintf(stderr, "WARN: Failed to load background image: %s\n",
            IMG_GetError());
  }

  update_viewport(context);
  return true;
}

void renderer_cleanup(RendererContext* context) {
  SDL_DestroyTexture(context->background_texture);
  SDL_DestroyTexture(context->game_texture);
  TTF_CloseFont(context->font_normal);
  TTF_CloseFont(context->font_large);
  TTF_CloseFont(context->font_small);
  SDL_DestroyRenderer(context->renderer);
  SDL_DestroyWindow(context->window);
  TTF_Quit();
  IMG_Quit();
}

void renderer_prepare_frame(RendererContext* context) {
  // Set the render target to our internal game texture. All subsequent draw
  // calls will render to this texture, not the window.
  SDL_SetRenderTarget(context->renderer, context->game_texture);

  // Clear it with a default "space black" color.
  SDL_SetRenderDrawColor(context->renderer, 10, 10, 20, 255);
  SDL_RenderClear(context->renderer);

  // Draw the background first, covering the entire logical area.
  if (context->background_texture) {
    SDL_RenderCopy(context->renderer, context->background_texture, NULL, NULL);
  }
}

void renderer_present_frame(RendererContext* context) {
  // Reset the render target back to the window (the default).
  SDL_SetRenderTarget(context->renderer, NULL);

  // Update viewport in case the window was resized since the last frame.
  update_viewport(context);

  // Clear the window with black bars in case the aspect ratio doesn't match.
  SDL_SetRenderDrawColor(context->renderer, 0, 0, 0, 255);
  SDL_RenderClear(context->renderer);

  // Copy the fully rendered game texture to the window, scaling it to the
  // calculated viewport.
  SDL_RenderCopy(context->renderer, context->game_texture, NULL,
                 &context->viewport);

  // Present the final frame to the user.
  SDL_RenderPresent(context->renderer);
}

void renderer_draw_game(RendererContext* context, const World* world) {
  // Draw projectiles first, so they appear behind other entities.
  for (int i = 0; i < MAX_PROJECTILES; i++) {
    if (world->projectiles[i].active) {
      const Projectile* p = &world->projectiles[i];
      SDL_SetRenderDrawColor(context->renderer, p->color.r, p->color.g,
                             p->color.b, 255);
      SDL_Rect rect = {(int)p->x - p->radius, (int)p->y - p->radius,
                       p->radius * 2, p->radius * 2};
      SDL_RenderFillRect(context->renderer, &rect);
    }
  }

  // Draw enemies.
  SDL_SetRenderDrawColor(context->renderer, 255, 0, 0, 255);
  for (int i = 0; i < MAX_ENEMIES; i++) {
    if (world->enemies[i].active) {
      const Enemy* e = &world->enemies[i];
      SDL_Rect rect = {(int)e->x - e->radius, (int)e->y - e->radius,
                       e->radius * 2, e->radius * 2};
      SDL_RenderFillRect(context->renderer, &rect);
    }
  }

  // Draw player last, so it appears on top.
  SDL_SetRenderDrawColor(context->renderer, 0, 255, 0, 255);
  SDL_Rect player_rect = {(int)world->player.x - world->player.radius,
                          (int)world->player.y - world->player.radius,
                          world->player.radius * 2, world->player.radius * 2};
  SDL_RenderFillRect(context->renderer, &player_rect);
}

void renderer_draw_hud(RendererContext* context, const World* world) {
  SDL_Color white = {255, 255, 255, 255};
  char buffer[64];

  // Draw score on the top-left.
  snprintf(buffer, sizeof(buffer), "Score: %d", world->score);
  render_text(context->renderer, context->font_normal, buffer, 10, 10, white,
              false);

  // Draw lives on the top-right.
  snprintf(buffer, sizeof(buffer), "Lives: %d", world->player.lives);
  int w, h;
  TTF_SizeText(context->font_normal, buffer, &w, &h);
  render_text(context->renderer, context->font_normal, buffer,
              LOGICAL_WIDTH - w - 10, 10, white, false);
}

void renderer_draw_menu(RendererContext* context, int selected_option) {
  SDL_Color white = {255, 255, 255, 255};
  SDL_Color green = {0, 255, 0, 255};  // Highlight color for selected option.

  render_text(context->renderer, context->font_large, "Starfall 2D",
              LOGICAL_WIDTH / 2, LOGICAL_HEIGHT / 4, white, true);
  render_text(context->renderer, context->font_normal, "Play",
              LOGICAL_WIDTH / 2, LOGICAL_HEIGHT / 2,
              selected_option == 0 ? green : white, true);
  render_text(context->renderer, context->font_normal, "Exit",
              LOGICAL_WIDTH / 2, LOGICAL_HEIGHT / 2 + 40,
              selected_option == 1 ? green : white, true);
  render_text(context->renderer, context->font_small,
              "Use ARROWS/ENTER or MOUSE", LOGICAL_WIDTH / 2,
              LOGICAL_HEIGHT - 30, white, true);
}

void renderer_draw_game_over(RendererContext* context, const World* world,
                             int selected_option) {
  // Draw a semi-transparent overlay to dim the background.
  SDL_SetRenderDrawBlendMode(context->renderer, SDL_BLENDMODE_BLEND);
  SDL_SetRenderDrawColor(context->renderer, 0, 0, 0, 180);
  SDL_Rect overlay = {0, 0, LOGICAL_WIDTH, LOGICAL_HEIGHT};
  SDL_RenderFillRect(context->renderer, &overlay);

  SDL_Color white = {255, 255, 255, 255};
  SDL_Color green = {0, 255, 0, 255};
  char score_text[64];
  snprintf(score_text, sizeof(score_text), "Final Score: %d", world->score);

  render_text(context->renderer, context->font_large, "Game Over",
              LOGICAL_WIDTH / 2, LOGICAL_HEIGHT / 4, white, true);
  render_text(context->renderer, context->font_normal, score_text,
              LOGICAL_WIDTH / 2, LOGICAL_HEIGHT / 4 + 60, white, true);
  render_text(context->renderer, context->font_normal, "Restart",
              LOGICAL_WIDTH / 2, LOGICAL_HEIGHT / 2 + 40,
              selected_option == 0 ? green : white, true);
  render_text(context->renderer, context->font_normal, "Main Menu",
              LOGICAL_WIDTH / 2, LOGICAL_HEIGHT / 2 + 80,
              selected_option == 1 ? green : white, true);
}

void renderer_toggle_fullscreen(RendererContext* context) {
  context->is_fullscreen = !context->is_fullscreen;
  Uint32 flags = context->is_fullscreen ? SDL_WINDOW_FULLSCREEN_DESKTOP : 0;
  SDL_SetWindowFullscreen(context->window, flags);
}

SDL_Rect renderer_get_text_rect(RendererContext* context, const char* text,
                                int x, int y) {
  if (!context->font_normal)
    return (SDL_Rect){0, 0, 0, 0};
  int w, h;
  // Use the normal font for button hitboxes for consistency.
  TTF_SizeText(context->font_normal, text, &w, &h);
  return (SDL_Rect){x - w / 2, y - h / 2, w, h};
}

void renderer_window_to_logical(RendererContext* context, int window_x,
                                int window_y, float* logical_x,
                                float* logical_y) {
  if (!logical_x || !logical_y)
    return;

  // Convert window coordinates to coordinates relative to the viewport.
  // This works for both letterboxing and stretching because the viewport
  // defines the drawable area of the game texture within the window.
  float relative_x = (float)(window_x - context->viewport.x);
  float relative_y = (float)(window_y - context->viewport.y);

  // Prevent division by zero if the window is minimized.
  if (context->viewport.w == 0 || context->viewport.h == 0) {
    *logical_x = 0;
    *logical_y = 0;
    return;
  }

  // Scale the relative coordinates back to the logical game resolution.
  *logical_x = relative_x * ((float)LOGICAL_WIDTH / context->viewport.w);
  *logical_y = relative_y * ((float)LOGICAL_HEIGHT / context->viewport.h);
}

// --- Private Helper Implementations ---

/**
 * @brief Renders a text string to the current render target.
 * @param renderer The active SDL_Renderer.
 * @param font The TTF_Font to use for rendering.
 * @param text The null-terminated string to render.
 * @param x The target X coordinate.
 * @param y The target Y coordinate.
 * @param color The SDL_Color for the text.
 * @param centered If true, (x,y) is the center point; otherwise, it's the
 * top-left corner.
 */
static void render_text(SDL_Renderer* renderer, TTF_Font* font,
                        const char* text, int x, int y, SDL_Color color,
                        bool centered) {
  if (!font || !text)
    return;
  SDL_Surface* surface = TTF_RenderText_Blended(font, text, color);
  if (!surface)
    return;
  SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
  if (!texture) {
    SDL_FreeSurface(surface);
    return;
  }
  SDL_Rect dest_rect = {x, y, surface->w, surface->h};
  if (centered) {
    dest_rect.x -= surface->w / 2;
    dest_rect.y -= surface->h / 2;
  }
  SDL_RenderCopy(renderer, texture, NULL, &dest_rect);
  SDL_DestroyTexture(texture);
  SDL_FreeSurface(surface);
}

/**
 * @brief Recalculates the rendering viewport based on the current window size.
 * This version implements a stretch-to-fill scaling strategy.
 * @param context A pointer to the RendererContext.
 */
static void update_viewport(RendererContext* context) {
  int window_w, window_h;
  SDL_GetRendererOutputSize(context->renderer, &window_w, &window_h);

  // Stretch-to-fill: The viewport is simply the entire window.
  // This will cause distortion if the window aspect ratio differs from the
  // logical aspect ratio, but it ensures no black bars.
  context->viewport = (SDL_Rect){0, 0, window_w, window_h};
}
