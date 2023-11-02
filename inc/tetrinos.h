#ifndef TETRINOS_H
#define TETRINOS_H
#include <cstdlib>
#include <cstdint>

struct Tetrino
{
    uint8_t *data; // Matrix representing the tetrino shape
    int32_t side;  // Dimensions of the matrix where data is assumed to be a square matrix

    Tetrino(const uint8_t *d = NULL, int32_t s = 1){
        data = (uint8_t*)d;
        side = s;
    }
};

// Bar tetrino
const uint8_t TETRINO1[]{
    0, 0, 0, 0,
    1, 1, 1, 1,
    0, 0, 0, 0,
    0, 0, 0, 0
};

// Small square tetrino
const uint8_t TETRINO2[]{
    2, 2,
    2, 2
};

// T shaped tetrino
const uint8_t TETRINO3[]{
    0, 0, 0,
    3, 3, 3,
    0, 3, 0
};

// S shaped tetrino
const uint8_t TETRINO4[]{
    0, 4, 4,
    2, 2, 0,
    0, 0, 0
};

// Z shaped tetrino
const uint8_t TETRINO5[]{
    4, 4, 0,
    0, 4, 4,
    0, 0, 0
};

// L shaped tetrino
const uint8_t TETRINO6[]{
    6, 0, 0,
    6, 6, 6,
    0, 0, 0
};

// Reverse L shaped tetrino
const uint8_t TETRINO7[]{
    0, 0, 7,
    7, 7, 7,
    0, 0, 0
};

const Tetrino TETRINOS[]{
    Tetrino(TETRINO1, 4),
    Tetrino(TETRINO2, 2),
    Tetrino(TETRINO3, 3),
    Tetrino(TETRINO4, 3),
    Tetrino(TETRINO5, 3),
    Tetrino(TETRINO6, 3),
    Tetrino(TETRINO7, 3)

};

#endif /* TETRINOS_H */