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
    
    if(keystate[SDL_SCANCODE_W]) player->y -= PLAYER_SPEED;
    if(keystate[SDL_SCANCODE_S]) player->y += PLAYER_SPEED;
    if(keystate[SDL_SCANCODE_A]) player->x -= PLAYER_SPEED;
    if(keystate[SDL_SCANCODE_D]) player->x += PLAYER_SPEED;

    // Limites da tela
    player->x = fmax(player->radius, fmin(SCREEN_WIDTH - player->radius, player->x));
    player->y = fmax(player->radius, fmin(SCREEN_HEIGHT - player->radius, player->y));
}