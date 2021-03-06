#ifndef PROMPT_H_
#define PROMPT_H_
#include "Card.h"
#include "Player.h"

void Prompt_display_hud(const Player* player);
void Prompt_display_suite(const suite_t suite);
void Prompt_display_face(const face_t face);
void Prompt_display_card(const struct card card);

/***
    Prompts the user to select an item from the list.
    The user will be prompted to pick between a number of 1 and option_count.
    Returns the index, aka user input subtracted by 1, of the chosen option.
    Returns a value less than 0 if an error occurred.
***/

int Prompt_get_target_option(const char* message, const char** option_list, const int option_count);
/***
    Prompts the user to select a card from a list. It takes a list of flags
    as to which card to prompt for, correlating to the use of declarations in
    Card.h to use face values as an index.
    A non-zero value at the card slot will result in the user being prompted
    for that card. For example:
        int my_cards[NUM_FACE_CARDS];
        memset(my_cards, 0, sizeof(my_cards));
        my_cards[CARD_10] = 1;
        my_cards[CARD_ACE] = 1;
        Prompt_pick_a_card(my_cards)
    will result in the user being to prompt between picking an ace or a 10
    Returns the face value of the selected card.
***/
face_t Prompt_pick_a_card(const int options[NUM_FACE_CARDS]);

/***
    Reads an entire line from stdin. It returns the first non-whitespace
    character. If the line is empty, it will continue reading to the first line
    with a non-whitespace character.
***/
char Prompt_getline_char(void);

/***
    Reads an entire line from stdin. It returns the first positive integer
    encountered. if the line is empty, or does not contain a positive integer,
    it will continue reading until reaching the first line with a positive
    integer.
***/
unsigned int Prompt_getline_uint(void);

/***
    Blocks until the user presses the enter key
***/
void Prompt_confirm_turn(void);
#endif
