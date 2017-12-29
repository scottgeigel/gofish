#include "GameController.h"
#include "Prompt.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int do_query_player(GameController* this);
static void deal_card(GameController* this, Player* player);
static void deal_5_cards(GameController* this, Player* to);
static void next_player(GameController* this);

void GameController_init(GameController* this, int num_players, const char **player_names) {
    int i, j;
    char player_name[PLAYER_MAX_NAME + 1];
    this->num_players = num_players;
    this->current_player = 0;
    for (i = 0; i < this->num_players; i++) {
        if (player_names != NULL && player_names[i][0] != '\0') {
            strncpy(player_name, player_names[i], PLAYER_MAX_NAME);
        } else {
            sprintf(player_name, "Player #%d", i+1);
        }
        Player_init(&this->players[i], player_name);
    }
    Deck_init(&this->deck);
    /* deal each player 5 cards */
    for (i = 0; i < 5; i++) {
        for (j = 0; j < this->num_players; j++) {
            deal_card(this, &this->players[j]);
        }
    }
}

int GameController_game_over(const GameController* this) {
    int i;
    int someone_has_cards = 0;

    for (i = 0; i < this->num_players; i++) {
        someone_has_cards |= Player_has_cards(&this->players[i]);
        if (someone_has_cards) break;
    }
    return Deck_cards_remaining(&this->deck) == 0 && !someone_has_cards;
}

void GameController_next_turn(GameController* this) {
    Action action;
    Player* current_player = &this->players[this->current_player];
    int stay_on_turn;
    int previous_points = current_player->score;
    int score_diff;
    const int cards_in_deck = Deck_cards_remaining(&this->deck) > 0;
    const int cards_in_hand = Player_has_cards(current_player);

    stay_on_turn = 0;

    /* prompt the player to make a turn */
    printf("\n\n\n");
    Prompt_display_hud(current_player);
    action = Prompt_get_action(cards_in_deck, cards_in_hand);
    switch (action) {
        case Action_Query:
            if (do_query_player(this)) {
                /* stays on the same turn */
                stay_on_turn = 1;
                printf("%s gets to go again!!!\n", current_player->name);
            } else {
                printf("Go Fish\n");
                deal_card(this, current_player);
            }
            break;
        case Action_Draw:
            deal_card(this, current_player);
            break;
        case Action_Exit:
            printf("%s took the coward's way out\n", current_player->name);
            exit(EXIT_SUCCESS);
        case Action_Invalid:
            fprintf(stderr, "received invalid action\n");
            abort();
    }
    /* check to see if the player is out of cards */
    while (!Player_has_cards(current_player)) {
        deal_5_cards(this, current_player);
        /*
            check to see if the player happened to get a matching suite,
            this should really only happen if there were only 4 cards left
            in the deck, and they were all the same face value
        */
        Player_tally_score(current_player);
    }
    Player_tally_score(current_player);
    if (previous_points != current_player->score) {
        score_diff = current_player->score - previous_points;
        printf("%s got %d point%s\n", current_player->name, score_diff, (score_diff == 1)? "" : "s");
    }
    if (!stay_on_turn) {
        next_player(this);
    }
}

static int do_query_player(GameController* this) {
    const char *player_labels[MAX_PLAYERS];
    int player_map[MAX_PLAYERS];
    int card_map[NUM_FACE_CARDS];
    int valid_players;
    int i;
    int target_player;
    face_t target_card;
    suite_t taken_cards;
    struct card card_buf;
    int got_cards;

    got_cards = 0;
    /*
        scan through every player that is not the current player and determine
        if they have any cards left in their hand
    */
    valid_players = 0;
    for (i = 0; i < this->num_players; i++) {
        if (i == this->current_player) {
            /* skip this one because it is the current player */
            continue;
        } else if (Player_has_cards(&this->players[i])) {
            player_map[valid_players] = i;
            player_labels[valid_players] = this->players[i].name;
            valid_players++;
        } else {
            /* skip this one because they have no cards */
            continue;
        }
    }
    /* present the options to the player and get feedback */
    i = Prompt_get_target_option("Which player?", player_labels, valid_players);
    /* validate i, as an index, for our player_label/option map */
    if (i < 0) {
        fprintf(stderr, "an error occurred while getting the target player\n");
        abort();
    } else if (i > this->num_players) {
        fprintf(stderr, "received invalid target player (%d) from prompt", i);
        abort();
    } else {
        /* now that i has been validated, pull the target player out */
        target_player = player_map[i];
        /* build the list of cards the player has */
        memset(card_map, 0, sizeof(card_map));
        for (i = 0; i < NUM_FACE_CARDS; i++) {
            card_map[i] = this->players[this->current_player].hand[i] > 0;
        }
        target_card = Prompt_pick_a_card(card_map);

        printf("%s asks %s if they have any ", this->players[this->current_player].name, this->players[target_player].name);
        Prompt_display_face(target_card);
        printf("s\n");

        if (Player_has_face_card(&this->players[target_player], target_card)) {
            /* take cards from that player */
            taken_cards = Player_give_face_cards_to(&this->players[this->current_player], &this->players[target_player], target_card);
            /* display the cards that the current player has acquired */
            card_buf.face = target_card;
            printf("You got: \n");
            if (taken_cards & SPADE) {
                putchar('\t');
                card_buf.suite = SPADE;
                Prompt_display_card(card_buf);
                putchar('\n');
            }
            if (taken_cards & CLUB) {
                putchar('\t');
                card_buf.suite = CLUB;
                Prompt_display_card(card_buf);
                putchar('\n');
            }
            if (taken_cards & HEART) {
                putchar('\t');
                card_buf.suite = HEART;
                Prompt_display_card(card_buf);
                putchar('\n');
            }
            if (taken_cards & DIAMOND) {
                putchar('\t');
                card_buf.suite = DIAMOND;
                Prompt_display_card(card_buf);
                putchar('\n');
            }
            /* check to see if we took away the last cards */
            if (!Player_has_cards(&this->players[target_player])) {
                deal_5_cards(this, &this->players[target_player]);
            }
            got_cards = 1;
        } else {
            /* go fish */
            got_cards = 0;
        }
    }
    return got_cards;
}

static void deal_card(GameController* this, Player* player) {
    struct card card = Deck_draw(&this->deck);
    printf("%s got a ", player->name);
    Prompt_display_card(card);
    putchar('\n');
    Player_give_card(player, card);
}

static void deal_5_cards(GameController* this, Player* to) {
    int i;
    for (i = 0; i < 5; i++) {
        /* do we have cards left in the deck? */
        if (Deck_cards_remaining(&this->deck) > 0) {
            deal_card(this, to);
        } else {
            /* no more cards left */
            break;
        }
    }
}

static void next_player(GameController* this) {
    this->current_player = (this->current_player + 1) % this->num_players;
}
