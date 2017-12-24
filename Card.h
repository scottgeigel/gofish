#ifndef CARD_H_
#define CARD_H_

typedef unsigned short int suite_t;
typedef unsigned short int face_t;

extern const suite_t SPADE;
extern const suite_t CLUB;
extern const suite_t HEART;
extern const suite_t DIAMOND;

extern const face_t CARD_1;
extern const face_t CARD_2;
extern const face_t CARD_3;
extern const face_t CARD_4;
extern const face_t CARD_5;
extern const face_t CARD_6;
extern const face_t CARD_7;
extern const face_t CARD_8;
extern const face_t CARD_9;
extern const face_t CARD_JACK;
extern const face_t CARD_QUEEN;
extern const face_t CARD_KING;
extern const face_t CARD_ACE;

struct card {
    suite_t suite;
    face_t face;
};

#endif
