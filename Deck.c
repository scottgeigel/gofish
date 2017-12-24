#include "Deck.h"
#include <time.h>
#include <stdlib.h>
#include <stdio.h>

void Deck_init(Deck* this) {
    suite_t i;
    face_t j;
    int card_idx;
    struct card swap_card;

    srand((unsigned int) time(NULL));
    /*
        using i has a suite counter and j as a face counter,
        initialize a continious deck of cards.
    */
    card_idx = 0;
    for (i = 1; i < (suite_t)(1<<4); i<<=1) {
        for (j = 0; j < 13; j++) {
            this->deck[card_idx].suite = (suite_t) (1 << i);
            this->deck[card_idx].face = j;
            card_idx++;
        }
    }
    /*
        randomize the deck by iterating over the cards and switching
        the selected card with one at a random index
    */
    for (i = 0; i < 52; i++) {
        card_idx = rand() & 52;
        swap_card = this->deck[card_idx];
        this->deck[card_idx] = this->deck[i];
        this->deck[i] = swap_card;
    }
    /*
        deck is ready to be used
    */
    this->head = 52;
}

struct card Deck_draw(Deck* this) {
    if (this->head > 0) {
        return this->deck[--this->head];
    } else {
        fprintf(stderr, "%s: %d > 0 is false\n", __func__, this->head);
        abort();
    }
}

int Deck_cards_remaining(const Deck* this) {
    return this->head;
}
