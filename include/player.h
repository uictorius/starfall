#ifndef PLAYER_H
#define PLAYER_H

#include "config.h"

typedef struct Player {
    float x, y;
    int radius;
    int lives;
} Player;

void init_player(Player *player);
void update_player(Player *player);

#endif