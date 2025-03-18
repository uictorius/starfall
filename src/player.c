#include "../include/player.h"
#include "../include/config.h"
#include "../include/game.h"

void init_player(Player *player) {
    player->x = SCREEN_WIDTH / 2;
    player->y = SCREEN_HEIGHT / 2;
    player->radius = 15;
    player->lives = 3;
}

void update_player(Player *player) {
    const Uint8 *keystate = SDL_GetKeyboardState(NULL);
    
    if (keystate[SDL_SCANCODE_W]) player->y = fmax(player->y - PLAYER_SPEED, player->radius);
    if (keystate[SDL_SCANCODE_S]) player->y = fmin(player->y + PLAYER_SPEED, SCREEN_HEIGHT - player->radius);
    if (keystate[SDL_SCANCODE_A]) player->x = fmax(player->x - PLAYER_SPEED, player->radius);
    if (keystate[SDL_SCANCODE_D]) player->x = fmin(player->x + PLAYER_SPEED, SCREEN_WIDTH - player->radius);
}