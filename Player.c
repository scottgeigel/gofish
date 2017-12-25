#include "Player.h"
#include <string.h>

void Player_init(Player* this, const char name[PLAYER_MAX_NAME + 1]) {
    this->score = 0;
    memset(&this->hand, 0, sizeof(this->hand));
    strncpy(this->name, name, PLAYER_MAX_NAME);
}

void Player_tally_score(Player* this) {
    static const suite_t card_mask = SPADE | CLUB | HEART | DIAMOND;
    int i;
    int score_diff = 0;

    for (i = 0; i < NUM_FACE_CARDS; i++) {
        /* have all suites been collected? */
        if ((this->hand[i] & card_mask) == card_mask) {
            /* remove the cards from hand */
            this->hand[i] = 0;
            score_diff++;
        }
    }
    this->score += score_diff;
}

void Player_give_card(Player* this, struct card card) {
    this->hand[card.face] |= card.suite;
}

int Player_has_cards(const Player* this) {
    int i;
    int has_cards = 0;

    for (i = 0; i < NUM_FACE_CARDS; i++) {
        if (this->hand[i] != 0) {
            has_cards = 1;
            break;
        }
    }
    return has_cards;
}

int Player_has_face_card(const Player* this, face_t face) {
    return this->hand[face] > 0;
}

void Player_give_face_cards_to(Player* to, Player* from, face_t face) {
    suite_t flags;
    flags = from->hand[face];
    from->hand[face] = 0;
    to->hand[face] |= flags;
}
