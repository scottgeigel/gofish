#ifndef GAMECONTROLLER_H_
#define GAMECONTROLLER_H_

#include "Deck.h"
#include "Player.h"

#define MAX_PLAYERS 4

typedef struct {
    int num_players;
    int current_player;
    Player players[MAX_PLAYERS];
    Deck deck;
} GameController;

void GameController_init(GameController* this, int num_players, const char **player_names);
int GameController_game_over(const GameController* this);
void GameController_next_turn(GameController* this);

#endif
