#ifndef PLAYER_H_
#define PLAYER_H_

#include "Card.h"
#define PLAYER_MAX_NAME 32
typedef unsigned char suite_flags_t;
typedef struct {
    int score;
    suite_t hand[NUM_FACE_CARDS];
    char name[PLAYER_MAX_NAME + 1];
    char __pad;
} Player;

void Player_init(Player* this, const char name[PLAYER_MAX_NAME + 1]);
void Player_tally_score(Player* this);
void Player_give_card(Player* this, struct card card);
int Player_has_cards(const Player* this);
int Player_has_face_card(const Player* this, face_t face);
void Player_give_face_cards_to(Player* to, Player* from, face_t face);
#endif
