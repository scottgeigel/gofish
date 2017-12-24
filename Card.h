#ifndef CARD_H_
#define CARD_H_

/***
    Represents a card's suite.
    Defines the values: SPADE, CLUB, HEART, DIAMOND
    These values are guaranteed to be suitable for masking so they be grouped
    like so:
        suite_t heart_and_diamond = HEART | DIAMOND;
        int has_SPADE = (value & SPADE) != 0;
***/
typedef unsigned short int suite_t;

/***
    Represents a card's face value
    Defines the values of CARD_([1-10],JACK,QUEEN,KING,ACE)
    The actual values range from 0 to 12. Therefore are suitable for indexing.

***/
typedef unsigned short int face_t;

extern const suite_t SPADE;
extern const suite_t CLUB;
extern const suite_t HEART;
extern const suite_t DIAMOND;

extern const face_t CARD_2;
extern const face_t CARD_3;
extern const face_t CARD_4;
extern const face_t CARD_5;
extern const face_t CARD_6;
extern const face_t CARD_7;
extern const face_t CARD_8;
extern const face_t CARD_9;
extern const face_t CARD_10;
extern const face_t CARD_JACK;
extern const face_t CARD_QUEEN;
extern const face_t CARD_KING;
extern const face_t CARD_ACE;

struct card {
    suite_t suite;
    face_t face;
};

#endif
