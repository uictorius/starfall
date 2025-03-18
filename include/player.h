#ifndef PLAYER_H
#define PLAYER_H

#include "config.h"

typedef struct GameState GameState;

typedef struct Player {
    float x, y;
    int radius;
    int lives;
} Player;

void init_player(Player *player, GameState *game);
void update_player(Player *player, GameState *game);

#endif