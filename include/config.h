#ifndef CONFIG_H
#define CONFIG_H

#include <stdbool.h>

#define DEFAULT_WIDTH 640
#define DEFAULT_HEIGHT 360
#define PLAYER_SPEED 5
#define PROJECTILE_SPEED 16
#define MAX_BOUNCES 3
#define FONT_SIZE 24
#define MAX_PROJECTILES 100
#define MAX_ENEMIES 50

typedef enum {
    GAME_STATE_MENU,
    GAME_STATE_PLAYING,
    GAME_STATE_GAME_OVER
} GameStateEnum;

#endif