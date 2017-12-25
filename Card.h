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

#define SPADE ((suite_t) 1<<0)
#define CLUB ((suite_t) 1<<1)
#define HEART ((suite_t) 1<<2)
#define DIAMOND ((suite_t) 1<<3)

#define CARD_2 ((face_t) 0)
#define CARD_3 ((face_t) 1)
#define CARD_4 ((face_t) 2)
#define CARD_5 ((face_t) 3)
#define CARD_6 ((face_t) 4)
#define CARD_7 ((face_t) 5)
#define CARD_8 ((face_t) 6)
#define CARD_9 ((face_t) 7)
#define CARD_10 ((face_t) 8)
#define CARD_JACK ((face_t) 9)
#define CARD_QUEEN ((face_t) 10)
#define CARD_KING ((face_t) 11)
#define CARD_ACE ((face_t) 12)
#define CARD_MIN ((face_t) 0)
#define CARD_MAX ((face_t) 12)
#define NUM_FACE_CARDS 13

struct card {
    suite_t suite;
    face_t face;
};

#endif
