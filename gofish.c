#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <ctype.h>

#define SUITE_SPADE ((unsigned int) (1<<0))
#define SUITE_CLUB ((unsigned int) (1<<1))
#define SUITE_HEART ((unsigned int) (1<<2))
#define SUITE_DIAMOND ((unsigned int) (1<<3))
#define SUITE_MASK ((unsigned int) 0x000F)

#define CARD_1 ((unsigned int) (1))
#define CARD_2 ((unsigned int) (2))
#define CARD_3 ((unsigned int) (3))
#define CARD_4 ((unsigned int) (4))
#define CARD_5 ((unsigned int) (5))
#define CARD_6 ((unsigned int) (6))
#define CARD_7 ((unsigned int) (7))
#define CARD_8 ((unsigned int) (8))
#define CARD_9 ((unsigned int) (9))
#define CARD_JACK ((unsigned int) (10))
#define CARD_QUEEN ((unsigned int) (11))
#define CARD_KING ((unsigned int) (12))
#define CARD_ACE ((unsigned int) (13))
#define CARD_MASK ((unsigned int) 13)

typedef struct {
    unsigned int hand[CARD_MASK];
    int score;
} player_t;

typedef struct {
    player_t* players;
    int num_players;
    int current_player;
    int next_card;
    unsigned int deck[52];
} game_t;

unsigned int make_card(unsigned int suite, unsigned int face) {
    return (face << 4) | suite;
}

void extract_card(unsigned int card, unsigned int* suite, unsigned int* face) {
    *suite = card & SUITE_MASK;
    *face = (card >> 4) & CARD_MASK;
}

void make_deck(unsigned int deck[52]) {
    int i,j;
    int card_idx;
    unsigned int swap_card;
    srand(time(NULL));
    card_idx = 0;
    for (i = 0; i < 4; i++) {
        for (j = 0; j < 14; j++) {
            deck[card_idx++] = make_card(1 << i, j);
        }
    }

    for (i = 0; i < 52; i++) {
        card_idx = rand() & 52;
        swap_card = deck[card_idx];
        deck[card_idx] = deck[i];
        deck[i] = swap_card;
    }
}

unsigned int player_get_cards(player_t* player, int face) {
    return player->hand[face];
}

void player_give_card(player_t* player, unsigned int card) {
    unsigned int face, suite;
    extract_card(card, &suite, &face);
    player->hand[face] |= suite;
}

void player_give_cards(player_t* player, unsigned int suites, unsigned int face) {
    player->hand[face] |= face;
    if ((player->hand[face] & SUITE_MASK) == SUITE_MASK) {
        player->hand[face] = 0;
        player->score++;
    }
}

unsigned int game_gather_cards_from_players(game_t* game, int face) {
    int i;
    unsigned int cards = 0;

    for (i = 0; i < game->num_players; i++) {
        if (i == game->current_player) {
            continue;
        } else {
            cards |= game->players[i].hand[face];
            game->players[i].hand[face] = 0;
        }
    }
    return cards;
}

int game_deal_card(game_t* game, player_t* player) {
    unsigned int card;
    if (game->next_card < 52) {
        card = game->deck[game->next_card++];
        player_give_card(player, card);
        return 1;
    } else {
        return 0;
    }
}

int game_cards_left(game_t* game) {
    if (game->next_card >= 52) {
        return 0;
    } else {
        return 52 - game->next_card;
    }
}

void game_init(game_t* game, player_t* players, int num_players) {
    int i,j;
    game->players = players;
    game->num_players = num_players;
    game->current_player = 0;
    game->next_card = 0;
    make_deck(game->deck);
    for (i = 0; i < game->num_players; i++) {
        memset(&game->players[i], 0, sizeof(player_t));
    }
    for (i = 0; i < 5; i++) {
        for (j = 0; j < game->num_players; j++) {
            game_deal_card(game, &game->players[j]);
        }
    }
}


void game_next_turn(game_t* game){
    game->current_player = (game->current_player + 1) % game->num_players;
}

int game_eval(game_t* game) {
    return game_cards_left(game) == 0;
}

const player_t* game_get_winner(game_t* game) {
    int high_score = 0;
    player_t* ret = NULL;
    int i;
    for (i = 0; i < game->num_players; i++) {
        if (game->players[i].score > high_score) {
            high_score = game->players[i].score;
            ret = &game->players[i];
        }
        /*TODO: account for ties?*/
    }
    return ret;
}

int prompt(game_t* game) {
    int choices[13];
    int next_choice = 0;
    int i;
    int print_comma = 0;
    char buf;
    int valid_choice;
    unsigned int card_flags = 0;
    player_t* current_player = &game->players[game->current_player];

    memset(choices, -1, sizeof(choices));
    printf("Player %d's turn\n", game->current_player + 1);
    printf("You have:\n");
    for (i = 0; i < 13; i++) {
        print_comma = 0;
        if (current_player->hand[i]) {
            choices[next_choice++] = i;
            if (i < 10) {
                printf("\t%d (", i + 1);
            } else if (i == 10) {
                printf("\tjack (");
            } else if (i == 11) {
                printf("\tqueen (");
            } else if (i == 12) {
                printf("\tking (");
            } else if (i == 13) {
                printf("\tace (");
            } else {
                printf("debug error ");
            }
            if (current_player->hand[i] & SUITE_SPADE) {
                printf("spades");
                print_comma = 1;
            }
            if (current_player->hand[i] & SUITE_CLUB) {
                if (print_comma) {
                    printf(", ");
                }
                printf("clubs");
                print_comma = 1;
            }
            if (current_player->hand[i] & SUITE_HEART) {
                if (print_comma) {
                    printf(", ");
                }
                printf("hearts");
                print_comma = 1;
            }
            if (current_player->hand[i] & SUITE_DIAMOND) {
                if (print_comma) {
                    printf(", ");
                }
                printf("diamonds");
                print_comma = 1;
            }
            if (!print_comma) {
                printf("print debug error\n");
            }
            printf(")\n");
        }
    }

    printf("\nOptions:\nd) Draw Card\n");
    for (i = 0; i < next_choice; i++) {
        if (choices[i] < 10) {
            printf("%d) Do you have any %d's\n", choices[i], choices[i] + 1);
        } else if (choices[i] == 10) {
            printf("j) Do you have any jacks\n");
        } else if (choices[i] == 11) {
            printf("q) Do you have any queens\n) ");
        } else if (choices[i] == 12) {
            printf("k) Do you have any kings\n");
        } else if (choices[i] == 10) {
            printf("a) Do you have any aces\n");
        } else {
            printf("debug error 2");
        }
    }

    valid_choice = 0;
    while(!valid_choice) {
        valid_choice = 1;
        /*consume any whitespace characters*/
        while(isspace(buf = getchar()));
        switch (buf) {
            case 'd':
                /*draw a card*/
                printf("Give me a card\n");
                game_deal_card(game, current_player);
                break;
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
                i = (buf - '0') + 1;
                printf("Do you have any %d's\n", i);
                card_flags = game_gather_cards_from_players(game, i);
                if (card_flags) {
                    player_give_cards(current_player, card_flags, i);
                    printf("You got:\n");
                    if (card_flags & SUITE_SPADE) {
                        printf("\ta spade\n");
                    }
                    if (card_flags & SUITE_CLUB) {
                        printf("\ta club\n");
                    }
                    if (card_flags & SUITE_HEART) {
                        printf("\ta heart\n");
                    }
                    if (card_flags & SUITE_DIAMOND) {
                        printf("\ta diamond\n");
                    }
                } else {
                    printf("There weren't any of those\n");
                }
                break;
            case 'j':
                printf("Do you have any jacks\n");
                card_flags = game_gather_cards_from_players(game, 10);
                if (card_flags) {
                    player_give_cards(current_player, card_flags, 10);
                } else {
                    printf("There weren't any of those\n");
                }
                break;
            case 'q':
                printf("Do you have any queens\n");
                card_flags = game_gather_cards_from_players(game, 11);
                if (card_flags) {
                    player_give_cards(current_player, card_flags, 11);
                } else {
                    printf("There weren't any of those\n");
                }
                break;
            case 'k':
                printf("Do you have any kings\n");
                card_flags = game_gather_cards_from_players(game, 12);
                if (card_flags) {
                    player_give_cards(current_player, card_flags, 12);
                } else {
                    printf("There weren't any of those\n");
                }
                break;
            case 'a':
                printf("Do you have any aces\n");
                card_flags = game_gather_cards_from_players(game, 13);
                if (card_flags) {
                    player_give_cards(current_player, card_flags, 13);
                } else {
                    printf("There weren't any of those\n");
                }
                break;
            default:
                printf("invalid choice %c\n", buf);
                valid_choice = 0;
                break;
        }
    }
    return card_flags;
}

int main() {
    player_t players[2];
    game_t game;
    char buf;
    game_init(&game, players, sizeof(players) / sizeof(player_t));
    while (!game_eval(&game)) {
        if (prompt(&game) == 0) {
            game_next_turn(&game);
        }
    }
    printf("Winner!!! Player %d\nScore: %d\n", game.current_player, game_get_winner(&game)->score);
}
