#ifndef TETROMINOS_H
#define TETROMINOS_H
#include <cstdlib>
#include <cstdint>

struct Tetromino
{
    uint8_t *data; // Matrix representing the tetromino shape
    int32_t side;  // Dimensions of the matrix where data is assumed to be a square matrix

    Tetromino(const uint8_t *d = NULL, int32_t s = 1){
        data = (uint8_t*)d;
        side = s;
    }
};

// Bar tetromino
const uint8_t TETROMINO1[]{
    0, 0, 0, 0,
    1, 1, 1, 1,
    0, 0, 0, 0,
    0, 0, 0, 0
};

// Small square tetromino
const uint8_t TETROMINO2[]{
    2, 2,
    2, 2
};

// T shaped tetromino
const uint8_t TETROMINO3[]{
    0, 0, 0,
    3, 3, 3,
    0, 3, 0
};

// S shaped tetromino
const uint8_t TETROMINO4[]{
    0, 4, 4,
    2, 2, 0,
    0, 0, 0
};

// Z shaped tetromino
const uint8_t TETROMINO5[]{
    4, 4, 0,
    0, 4, 4,
    0, 0, 0
};

// L shaped tetromino
const uint8_t TETROMINO6[]{
    6, 0, 0,
    6, 6, 6,
    0, 0, 0
};

// Reverse L shaped tetromino
const uint8_t TETROMINO7[]{
    0, 0, 7,
    7, 7, 7,
    0, 0, 0
};

const Tetromino TETROMINOS[]{
    Tetromino(TETROMINO1, 4),
    Tetromino(TETROMINO2, 2),
    Tetromino(TETROMINO3, 3),
    Tetromino(TETROMINO4, 3),
    Tetromino(TETROMINO5, 3),
    Tetromino(TETROMINO6, 3),
    Tetromino(TETROMINO7, 3)

};

#endif /* TETROMINOS_H */