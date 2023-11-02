#ifndef TETRIS_H
#define TETRIS_H

#include <cstdio>
#include <cstring>
#include "./tetrinos.h"
#include "./colors.h"
using namespace std;

/*
Height and width are taken from Nintendo Tetris's wiki page
*/
#define WIDTH 14
#define HEIGHT 22         // Adding two hidden rows to spawn the pieces
#define VISIBLE_HEIGHT 20 // Actual board height
#define GRID_SIZE 30
#define ARRAY_COUNT(x) (sizeof(x) / sizeof(x)[0])

// Frames taken from Nintendo Tetris's wiki page
const uint8_t FRAMES_PER_DROP[] = {
    48,
    43,
    38,
    33,
    28,
    23,
    18,
    13,
    8,
    6,
    5,
    5,
    5,
    4,
    4,
    4,
    3,
    3,
    3,
    2,
    2,
    2,
    2,
    2,
    2,
    2,
    2,
    2,
    2,
    1
};

// Standard frame rate is 60 frames per second and its reciprocal is time
const float TARGET_SECONDS_PER_FRAME = 1.0 / 60.0;

enum GamePhase {
    GAME_PHASE_START,
    GAME_PHASE_PLAY,
    GAME_PHASE_LINE,
    GAME_PHASE_GAMEOVER
};

enum TextAlign{
    TEXT_ALIGN_LEFT,
    TEXT_ALIGN_CENTER,
    TEXT_ALIGN_RIGHT
};

struct PieceState
{
    uint8_t tetrinoIndex; // Index indication which tetrino
    int32_t offsetRow;    // Row of the tetrino
    int32_t offsetCol;    // Column of the tetrino
    int32_t rotation;     // Rotation of the tetrino
};

struct GameState
{
    uint8_t board[WIDTH * HEIGHT];
    uint8_t lines[HEIGHT];  // Stores the number of lines that are filled

    PieceState piece;
    GamePhase phase;
    
    int32_t level;      // Current level of the game
    int32_t startLevel; // Start level of the game
    
    int32_t lineCount; 
    int32_t pendingLineCount;
    int32_t score;

    float nextDropTime; // Next drop time of the tetrino piece
    float time;         // Current time
    float highlightEndTime;
};

struct InputState
{
    uint8_t left;
    uint8_t right;
    uint8_t up;
    uint8_t down;
    uint8_t a;

    int8_t deltaLeft;
    int8_t deltaRight;
    int8_t deltaUp;
    int8_t deltaDown;
    int8_t deltaA;
};

// Function Prototypes
uint8_t tetrino_get(const Tetrino *tetrino, int32_t row, int32_t col, int32_t rotation);

uint8_t matrix_get(const uint8_t *values, int32_t width, int32_t row, int32_t col);
void matrix_set(uint8_t *values, int32_t width, int32_t row, int32_t col, uint8_t value);

bool check_piece_valid(PieceState *piece, const uint8_t *board, int32_t width, int32_t height);
void merge_piece(GameState *game);
void spawn_piece(GameState *game);
bool soft_drop(GameState *game);

int32_t find_lines(const uint8_t *values, int32_t width, int32_t height, uint8_t *linesOut);
void clear_lines(const uint8_t* values, int32_t width, int32_t height, const uint8_t* lines);

void update_game_start(GameState *game, const InputState* input);
void update_game_line(GameState* game);
void update_game_gameover(GameState *game, const InputState* input);
void update_game_play(GameState *game, const InputState *input);
void update_game(GameState *game, const InputState *input);

#endif /*TETRIS_H*/