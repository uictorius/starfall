#include "game.h"

int main() {
    GameState game;
    initialize_game(&game);
    run_game_loop(&game);
    cleanup_game(&game);
    return 0;
}