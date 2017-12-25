#include "GameController.h"
#include <stdio.h>
#include <stdlib.h>

int main() {
    /* TODO: implement a mechanism to select players */
    GameController game;
    int num_players = 2;
    GameController_init(&game, num_players, NULL);
    while (!GameController_game_over(&game)) {
        GameController_next_turn(&game);
    }
    printf("game over\n");
}
