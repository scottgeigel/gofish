#ifndef DECK_H_
#define DECK_H_

#include "Card.h"

typedef struct {
    int head;
    struct card deck[52];
} Deck;

void Deck_init(Deck* this);
struct card Deck_draw(Deck* this);
int Deck_cards_remaining(const Deck* this);
#endif
