#include "../include/player.h"
#include "../include/config.h"
#include "../include/game.h"

void init_player(Player *player, GameState *game) {
    player->x = game->current_width / 2;
    player->y = game->current_height / 2;
    player->radius = 12;
    player->lives = 3;
}

void update_player(Player *player, GameState *game) {
    const Uint8 *keystate = SDL_GetKeyboardState(NULL);
    
    if (keystate[SDL_SCANCODE_W]) player->y = fmax(player->y - PLAYER_SPEED, player->radius);
    if (keystate[SDL_SCANCODE_S]) player->y = fmin(player->y + PLAYER_SPEED, game->current_height - player->radius);
    if (keystate[SDL_SCANCODE_A]) player->x = fmax(player->x - PLAYER_SPEED, player->radius);
    if (keystate[SDL_SCANCODE_D]) player->x = fmin(player->x + PLAYER_SPEED, game->current_width - player->radius);
}