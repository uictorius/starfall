/**
 * @file renderer.h
 * @brief Defines the public interface for the rendering subsystem.
 *
 * This module handles window creation, asset loading (textures, fonts),
 * and all drawing operations. It implements a dynamic scaling architecture
 * to ensure the game viewport fills any window size, maintaining a consistent
 * logical resolution.
 */

#ifndef RENDERER_H
#define RENDERER_H

#include "game/world.h"
#include "utils/types.h"

// --- Public API ---

// Lifecycle Functions
/**
 * @brief Initializes the SDL window, renderer, and loads all graphical assets.
 * @param context A pointer to the RendererContext to be initialized.
 * @return true on successful initialization, false otherwise.
 */
bool renderer_init(RendererContext* context);

/**
 * @brief Frees all graphical assets and shuts down the rendering subsystem.
 * @param context A pointer to the RendererContext to be cleaned up.
 */
void renderer_cleanup(RendererContext* context);

// Frame Drawing Functions
/**
 * @brief Prepares for a new frame by setting the render target to the internal
 * game texture and clearing it.
 * @param context A pointer to the RendererContext.
 */
void renderer_prepare_frame(RendererContext* context);

/**
 * @brief Presents the final rendered frame to the screen.
 *
 * This function handles the dynamic scaling by rendering the internal game
 * texture to the window's viewport.
 * @param context A pointer to the RendererContext.
 */
void renderer_present_frame(RendererContext* context);

// State-specific Rendering
/**
 * @brief Renders the active gameplay scene, including all entities.
 * @param context A pointer to the RendererContext for drawing operations.
 * @param world A constant pointer to the game world containing entity data.
 */
void renderer_draw_game(RendererContext* context, const World* world);

/**
 * @brief Renders the Heads-Up Display (score, lives, etc.) over the game scene.
 * @param context A pointer to the RendererContext for drawing operations.
 * @param world A constant pointer to the game world for HUD data.
 */
void renderer_draw_hud(RendererContext* context, const World* world);

/**
 * @brief Renders the main menu scene.
 * @param context A pointer to the RendererContext for drawing operations.
 * @param selected_option The index of the currently highlighted menu option.
 */
void renderer_draw_menu(RendererContext* context, int selected_option);

/**
 * @brief Renders the game over screen.
 * @param context A pointer to the RendererContext for drawing operations.
 * @param world A constant pointer to the game world for displaying the final
 * score.
 * @param selected_option The index of the currently highlighted menu option.
 */
void renderer_draw_game_over(RendererContext* context, const World* world,
                             int selected_option);

// Utility Functions
/**
 * @brief Toggles the window between fullscreen desktop and windowed mode.
 * @param context A pointer to the RendererContext.
 */
void renderer_toggle_fullscreen(RendererContext* context);

/**
 * @brief Calculates the screen bounding box for a given text string.
 *
 * This is useful for UI hit detection, as it determines the clickable area
 * for a text-based button.
 * @param context A pointer to the RendererContext (to access fonts).
 * @param text The text string to measure.
 * @param x The logical X coordinate of the text's center.
 * @param y The logical Y coordinate of the text's center.
 * @return An SDL_Rect representing the text's bounding box in logical
 * coordinates.
 */
SDL_Rect renderer_get_text_rect(RendererContext* context, const char* text,
                                int x, int y);

/**
 * @brief Converts window coordinates (e.g., from a mouse click) to logical game
 * coordinates.
 *
 * This is essential for mouse interaction, allowing clicks on the resized
 * window to be accurately mapped to the game's fixed resolution.
 * @param context A pointer to the RendererContext (to access viewport data).
 * @param window_x The mouse's X coordinate within the window.
 * @param window_y The mouse's Y coordinate within the window.
 * @param logical_x A pointer to a float to store the resulting logical X
 * coordinate.
 * @param logical_y A pointer to a float to store the resulting logical Y
 * coordinate.
 */
void renderer_window_to_logical(RendererContext* context, int window_x,
                                int window_y, float* logical_x,
                                float* logical_y);

#endif  // RENDERER_H
