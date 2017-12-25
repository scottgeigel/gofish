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

    stay_on_turn = 0;
    /* prompt the player to make a turn */
    Prompt_display_hud(current_player);
    action = Prompt_get_action(Deck_cards_remaining(&this->deck) > 0);
    switch (action) {
        case Action_Query:
            if (do_query_player(this)) {
                /* stays on the same turn */
                stay_on_turn = 1;
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
        default:
            fprintf(stderr, "received invalid action\n");
            abort();
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
    target_player = Prompt_get_target_option("Which player?", player_labels, valid_players);
    if (target_player < 0) {
        fprintf(stderr, "an error occurred while getting the target player\n");
        abort();
    } else if (target_player >= this->num_players) {
        fprintf(stderr, "received invalid target player (%d) from prompt", target_player);
        abort();
    } else {
        /* build the list of cards the player has */
        memset(card_map, 0, sizeof(card_map));
        for (i = 0; i < NUM_FACE_CARDS; i++) {
            card_map[i] = this->players[this->current_player].hand[i] > 0;
        }
        target_card = Prompt_pick_a_card(card_map);
        if (Player_has_face_card(&this->players[target_player], target_card)) {
            /* take cards from that player */
            Player_give_face_cards_to(&this->players[target_player], &this->players[this->current_player], target_card);
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
    this->current_player = (this->current_player + 1) & this->num_players;
}
