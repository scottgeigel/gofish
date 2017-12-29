#ifndef lint
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#endif

#include "Prompt.h"

static void confirm_exit() {
    int choice;
try_again:
    printf("Do you really want to exit?\n");
    printf("y/n: ");
    choice = getchar();
    if (choice == 'y' || choice == 'Y') {
        printf("terminated\n");
        exit(EXIT_SUCCESS);
    } else if (choice == 'n' || choice == 'N') {
        /* consume the rest of the line */
        do { choice = getchar(); } while (choice != '\n');
        /* leave the function as the user doesn't really want to exit */
    } else {
        printf("I don't understande %c\n", choice);
        goto try_again;
    }
}
void Prompt_display_hud(const Player* player) {
    face_t i;
    int print_comma;

    printf("Player: %s\n", player->name);
    printf("Score: %d\n", player->score);
    printf("Hand:\n");

    for (i = CARD_MIN; i <= CARD_MAX; i++) {
        print_comma = 0;
        /* if the player has any suites for face (i) */
        if (player->hand[i] != 0) {
            putchar('\t');
            Prompt_display_face(i);
            printf(" (");
            /* check which suite flags have been set */
            if (player->hand[i] & SPADE) {
                Prompt_display_suite(SPADE);
                print_comma = 1;
            }
            if (player->hand[i] & CLUB) {
                if (print_comma) {
                    printf(", ");
                }
                Prompt_display_suite(CLUB);
                print_comma = 1;
            }
            if (player->hand[i] & HEART) {
                if (print_comma) {
                    printf(", ");
                }
                Prompt_display_suite(HEART);
                print_comma = 1;
            }
            if (player->hand[i] & DIAMOND) {
                if (print_comma) {
                    printf(", ");
                }
                Prompt_display_suite(DIAMOND);
            }
            printf(")\n");
        }
    }
}

void Prompt_display_suite(const suite_t suite) {
    switch (suite) {
        case SPADE:
            printf("Spade");
            break;
        case CLUB:
            printf("Club");
            break;
        case HEART:
            printf("Heart");
            break;
        case DIAMOND:
            printf("Diamond");
            break;
        default:
            fprintf(stderr, "unknown suite type 0x%02X\n", suite);
            abort();
    }
}

void Prompt_display_face(const face_t face) {
    switch (face) {
        case CARD_10:
            printf("10");
            break;
        case CARD_2:
            printf("2");
            break;
        case CARD_3:
            printf("3");
            break;
        case CARD_4:
            printf("4");
            break;
        case CARD_5:
            printf("5");
            break;
        case CARD_6:
            printf("6");
            break;
        case CARD_7:
            printf("7");
            break;
        case CARD_8:
            printf("8");
            break;
        case CARD_9:
            printf("9");
            break;
        case CARD_JACK:
            printf("Jack");
            break;
        case CARD_QUEEN:
            printf("Queen");
            break;
        case CARD_KING:
            printf("King");
            break;
        case CARD_ACE:
            printf("Ace");
            break;
        default:
            fprintf(stderr, "Unknown face type %d\n", face);
            abort();
    }
}

void Prompt_display_card(const struct card card) {
    Prompt_display_face(card.face);
    printf(" of ");
    Prompt_display_suite(card.suite);
    printf("'s");
}

Action Prompt_get_action(int can_draw, int can_ask) {
    Action action = Action_Invalid;
    char choice;
    /* assert we're in a valid prompt */
    if (!can_draw && !can_ask) {
        fprintf(stderr, "can_draw(%d) and can_ask(%d) cannot both be 0\n", can_draw, can_ask);
        abort();
    }
    /* loop until the user provides a valid response */
    while (action == Action_Invalid) {
        printf("Options:\n");
        if (can_draw) {
            printf("\td) Draw a card\n");
        }
        if (can_ask) {
            printf("\ta) Ask a player for a card\n");
        }
        printf("\te) exit\n");

        choice = Prompt_getline_char();;
        /*
            is the user is trying to close via  EOF, go ahead and translate to exit
        */
        if (choice == '\0') {
            action = Action_Exit;
        } else {
            /*
                match against known input
            */
            switch (choice) {
                case 'a':
                    if (can_ask) {
                        action = Action_Query;
                    }
                    break;
                case 'e':
                    action = Action_Exit;
                    break;
                case 'd':
                    if (can_draw) {
                        action = Action_Draw;
                    } /* else action will remain Action_Invalid and go to the invalid message */
                    break;
                default:
                    /* action will remain Action_Invalid and go to the invalid message */
                    break;
            }
        }
        if (action == Action_Invalid) {
            printf("%c is an invalid option\n", choice);
        }
    }
    return action;
}

int Prompt_get_target_option(const char* message, const char** option_list, const int option_count) {
    int i;
    int choice = -1;
    int final_selection_made = 0;

    while (!final_selection_made) {
        if (choice)
        /* display the options to the user */
        printf("%s\nOptions:\n", message);
        for (i = 0; i < option_count; i++) {
            printf("\t%d) %s\n", i+1, option_list[i]);
        }
        /* get the response from the user */
        choice = (int) Prompt_getline_uint(); /* range check below will take care of overlow */
        if (1 <= choice && choice <= option_count) {
            /*
                user input is in range of the option_list, conver to index
                and pass on to user
            */
            choice -= 1;
            final_selection_made = 1;
        } else {
            /* final selection not made, re-run the prompt */
            final_selection_made = 0;
            printf("Invalid selection %d\n", choice);
        }
    }
    return choice;
}

face_t Prompt_pick_a_card(const int options[NUM_FACE_CARDS]) {
    face_t option_map[NUM_FACE_CARDS];
    face_t idx;
    face_t ret = CARD_INVALID;
    int i;
    int option_count = 0;
    int valid_choice = 0;
    int choice;

    /* build the option map */
    for (idx = CARD_MIN; idx <= CARD_MAX; idx++) {
        if (options[idx]) {
            option_map[option_count] = idx;
            option_count++; /* prompt the user in non-zero based values */
        }
    }

    while (!valid_choice) {
        printf("Which card would you like?\n");
        for (i = 0; i < option_count; i++) {
            printf("\t%d) ", i + 1);
            Prompt_display_face(option_map[i]);
            putchar('\n');
        }
        /* assert that we actually have some valid options */
        if (option_count <= 0 || option_count > NUM_FACE_CARDS) {
            fprintf(stderr, "ended up with invalid option_count of %d\n", option_count);
            abort();
        }

        choice = (int) Prompt_getline_uint(); /* range check below will take care of overflow */
        if (choice <= option_count && choice > 0) { /* check to make sure there was a reasonable selection */
            valid_choice = 1;
            /* offset the choice, now that in a validated range, to an index */
            ret = option_map[choice - 1];
        } else {
            /* invalid choice, prompt again */
            valid_choice = 0;
            printf("invalid choice %d\n", choice);
        }
    }

    return ret;
}

char Prompt_getline_char() {
    int buf;
    char ret = '\0';
    /* skip all non printable characters */
    do {
        buf = getchar();
        /* make sure we exit on errors instead of running indefinitely */
        if (buf < 0) {
            confirm_exit();
        }
    } while (!isgraph(buf));

    /* now that we have a character, skip until the end of the line */
    ret = (char) buf;
    do {
        buf = getchar();
        /*
            make sure we exit on errors instead of hopelessly searching
            for the linefeed
        */
        if (buf < 0) {
            confirm_exit();
            break; /* it's ok to exit here */
        }
    } while (buf != '\n');

    return ret;
}

unsigned int Prompt_getline_uint() {
    unsigned int ret = 0;
    int buf;

    /* skip all non-numeric characters */
    do {
        buf = getchar();
        /* make sure we exit on errors instead of running indefinitely */
        if (buf < 0) {
            confirm_exit();
        }
    } while (!isdigit(buf));

    do {
        /* shift the digit on the accumulator and hope there isn't an overflow */
        ret *= 10;
        ret += (unsigned int) (buf - '0'); /* we're guarded by isdigit */
        buf = getchar();
        /* make sure we exit on errors instead of running indefinitely */
        if (buf < 0) {
            confirm_exit();
             /*
                number interrupted here, create a false newline, on the user's
                beahlf to exit the function.
            */
            buf = '\n';
            break;
        }
    } while (isdigit(buf));

    /* consume the rest of the charcters until the end of the line */
    while (buf != '\n') {
        buf = getchar();
        /*
            make sure we exit on errors instead of hopelessly searching
            for the linefeed
        */
        if (buf < 0) {
            confirm_exit();
            break; /* it's ok to exit here */
        }
    }

    return ret;
}
