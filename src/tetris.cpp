#include <cassert>
// #include <SDL2/SDL.h>
#include "../inc/tetris.h"

// Function Prototypes
inline int32_t generate_random_int(int32_t min, int32_t max);
inline float get_time_to_next_drop(int32_t gameLevel);
inline uint8_t check_row_filled(const uint8_t *values, int32_t width, int32_t row);
inline int32_t compute_score(int32_t level, int32_t lineCount);
inline int32_t get_lines_for_next_level(int32_t startLevel, int32_t currentLevel);
inline uint8_t check_row_empty(const uint8_t *values, int32_t width, int32_t row);

/**
 * @brief Gets the data from the tetrino considering rotation
 *
 * @param tetrino - received tetrino to get data from
 * @param row - row
 * @param col - column
 * @return uint8_t - data value
 */
uint8_t tetrino_get(const Tetrino *tetrino, int32_t row, int32_t col, int32_t rotation)
{
    int32_t side = tetrino->side;
    switch (rotation)
    {
    case 0:
        // No rotation
        return tetrino->data[row * side + col];
    case 1:
        /*
            Original matrix rotated by 90 degrees clockwise
            Original matrix row = (side - col - 1) in rotated matrix
            Original matrix col = row in rotated matrix
        */
        return tetrino->data[(side - col - 1) * side + row];
    case 2:
        /*
            Original matrix rotated by 180 degrees clockwise
            Original matrix row = (side - row - 1) in rotated matrix
            Original matrix col = (side - col - 1) in rotated matrix
        */
        return tetrino->data[(side - row - 1) * side + (side - col - 1)];
    case 3:
        /*
            Original matrix rotated by 270 degrees clockwise
            Original matrix row = (col) in rotated matrix
            Original matrix col = (side - row - 1) in rotated matrix
        */
        return tetrino->data[col * side + (side - row - 1)];
    default:
        return 0;
    }
    return 0;
}

/**
 * @brief Gets value at the computed index from the board
 *
 * @param values - matrix to get the value from
 * @param width - width of the board
 * @param row - row of the board
 * @param col - column of the board
 * @return uint8_t - value at the computed index
 */
uint8_t matrix_get(const uint8_t *values, int32_t width, int32_t row, int32_t col)
{
    int32_t index = row * width + col;
    return values[index];
}

/**
 * @brief Sets the value on the board matrix
 *
 * @param values - Received value is stored in this matrix at the computed index
 * @param width - width of the board
 * @param row - row of the board
 * @param col - column of the board
 * @param value - value to be stored
 */
void matrix_set(uint8_t *values, int32_t width, int32_t row, int32_t col, uint8_t value)
{
    int32_t index = row * width + col;
    values[index] = value;
}

/**
 * @brief - Checks whether the piece is valid or not and returns true/false accordingly
 * Moves that are not permitted:
 * (i) If piece's movements (left, right, drop faster) goes out of bounds or collides
 * (ii) If piece's rotation overlaps with something else on the board
 *
 * @param piece - Pointer to PieceState
 * @param board - board array
 * @param width - width of the board
 * @param height - height of the board
 * @return true - valid piece
 * @return false - invalid piece
 */
bool check_piece_valid(PieceState *piece, const uint8_t *board, int32_t width, int32_t height)
{
    const Tetrino *tetrino = TETRINOS + piece->tetrinoIndex;
    assert(tetrino);

    // Loop through all the cells of the tetrino and check
    // whether the non-empty (non-zero) cells collide or
    // are out-of-bounds.
    for (int32_t row = 0; row < tetrino->side; row++)
    {
        for (int32_t col = 0; col < tetrino->side; col++)
        {
            uint8_t value = tetrino_get(tetrino, row, col, piece->rotation);
            if (value > 0)
            {

                // Computing corresponding board's row and column
                // from the piece's row and column
                int32_t boardRow = piece->offsetRow + row;
                int32_t boardCol = piece->offsetCol + col;

                // Invalid scenario - out of bounds
                if ((boardRow < 0) || (boardRow >= height))
                {
                    return false;
                }
                if ((boardCol < 0) || (boardCol >= width))
                {
                    return false;
                }
                // Invalid scenario - collision detected if the matrix_get() returns a value   for the current boardRow and boardCol
                if (matrix_get(board, width, boardRow, boardCol))
                {
                    return false;
                }
            }
        }
    }

    // Else true
    return true;
}

/**
 * @brief Merges the collided piece with the board by copying its contents onto the board
 *
 * @param game - pointer to GameState holding the current state of the game
 */
void merge_piece(GameState *game)
{
    const Tetrino *tetrino = TETRINOS + game->piece.tetrinoIndex;
    for (int32_t row = 0; row < tetrino->side; row++)
    {
        for (int32_t col = 0; col < tetrino->side; col++)
        {
            uint8_t value = tetrino_get(tetrino, row, col, game->piece.rotation);
            if (value)
            {
                int32_t boardRow = game->piece.offsetRow + row;
                int32_t boardCol = game->piece.offsetCol + col;
                matrix_set(game->board, WIDTH, boardRow, boardCol, value);
            }
        }
    }
}

inline int32_t generate_random_int(int32_t min, int32_t max)
{
    int32_t range = max - min;
    return min + rand() % range;
}

/**
 * @brief Computes and returns the time for next tetrino piece to drop based on the current level
 * Information about the game level is taken from Nintendo Tetris's wiki page
 *
 * @param gameLevel - current level of the game
 * @return float - time for next drop
 */
inline float get_time_to_next_drop(int32_t gameLevel)
{
    if (gameLevel > 29)
    {
        gameLevel = 29;
    }
    return FRAMES_PER_DROP[gameLevel] * TARGET_SECONDS_PER_FRAME;
}

/**
 * @brief Spawns a new tetrino peice generated randomly
 *
 * @param game - pointer to GameState holding the current state of the game
 */
void spawn_piece(GameState *game)
{
    game->piece = {};
    game->piece.tetrinoIndex = static_cast<uint8_t>(generate_random_int(0, ARRAY_COUNT(TETRINOS)));
    game->piece.offsetCol = WIDTH / 2;
    game->nextDropTime = game->time + get_time_to_next_drop(game->level);
}

/**
 * @brief - The tetrino piece is moved down. If the piece collides with the board, the piece is moved back up by one step and is fixed there by copying the contents of the piece's contents. Finally, a new tetrino piece is spawned and the function ends by returning true/false.
 *
 * @param game - pointer to GameState holding the current state of the game
 * @return true - next drop occurs
 * @return false - spawned a new piece and next drop did not occur
 */
bool soft_drop(GameState *game)
{
    // Move the piece down by incrementing its row offset
    game->piece.offsetRow++;

    // If piece is invalid then collision occurred
    if (!check_piece_valid(&game->piece, game->board, WIDTH, HEIGHT))
    {
        // Move the piece up by decrementing its row offset
        game->piece.offsetRow--;

        // Merge the piece with the board
        merge_piece(game);

        // Spawn a new tetrino piece
        spawn_piece(game);
        return false;
    }

    game->nextDropTime = game->time + get_time_to_next_drop(game->level);
    return true;
}

/**
 * @brief - Checks whether the lines are filled or not.
 *
 * @param values - array containing board data
 * @param width - width of the board
 * @param row - number of rows in the board
 * @return uint8_t - 1 indicates filled while 0 indicates empty
 */
inline uint8_t check_row_filled(const uint8_t *values, int32_t width, int32_t row)
{
    for (int32_t col = 0; col < width; col++)
    {
        if (!matrix_get(values, width, row, col))
        {
            return 0;
        }
    }
    return 1;
}

/**
 * @brief - Finds lines on the board that are filled and populates an array containing height elements with 1 or 0. 1 indicates that the line (row) is filled while 0 indicates empty. It returns the number of filled lines.
 *
 * @param values - array containing board data
 * @param width - width of the board
 * @param height - height of the board
 * @param linesOut - array of length height that contains the number of filled rows
 * @return int32_t - number of filled lines
 */
int32_t find_lines(const uint8_t *values, int32_t width, int32_t height, uint8_t *linesOut)
{
    int32_t count = 0;
    for (int32_t row = 0; row < height; row++)
    {
        uint8_t filled = check_row_filled(values, width, row);
        linesOut[row] = filled;
        count += filled;
    }
    return count;
}

/**
 * @brief Clears the filled lines by copying the filled lines from the source matrix to the destination matrix
 *
 * @param values - array containing board information
 * @param width - width of the board
 * @param height - height of the board
 * @param lines - array containing number of filled lines
 */
void clear_lines(const uint8_t *values, int32_t width, int32_t height, const uint8_t *lines)
{
    int32_t srcRow = height - 1;
    for (int32_t destRow = height - 1; destRow >= 0; destRow--)
    {

        // Decrementing srcRow as long as the lines[srcRow] is filled (i.e 1)
        while (srcRow > 0 && lines[srcRow])
        {
            srcRow--;
        }
        if (srcRow < 0)
        {
            memset((uint8_t *)values + destRow * width, 0, width);
        }
        else
        {
            memcpy((uint8_t *)values + destRow * width, values + srcRow * width, width);
            srcRow--;
        }
    }
}

/**
 * @brief Computes the game score based on the line count and current game level and, returns the score
 * Information about the scoring system is taken from Nintendo Tetris's wiki page
 *
 * @param level - current level of the game
 * @param lineCount - number of lines filled
 * @return int32_t - game score
 */
inline int32_t compute_score(int32_t level, int32_t lineCount)
{
    switch (lineCount)
    {
    case 1:
        return 40 * (level + 1);
    case 2:
        return 100 * (level + 1);
    case 3:
        return 300 * (level + 1);
    case 4:
        return 1200 * (level + 1);
    }
    return 0;
}

inline int32_t min(int32_t x, int32_t y)
{
    return x < y ? x : y;
}

inline int32_t max(int32_t x, int32_t y)
{
    return x > y ? x : y;
}

/**
 * @brief Get the lines required to switch to next level
 * Information about the switching to next level is taken from Nintendo Tetris's wiki page
 *
 * @param startLevel - start level of the game
 * @param currentLevel - current level of the game
 * @return int32_t - number of lines required for the next level
 */
inline int32_t get_lines_for_next_level(int32_t startLevel, int32_t currentLevel)
{
    int32_t firstLevelUpLimit = min((startLevel * 10 + 10), max(100, (startLevel * 10 - 50)));
    if (currentLevel == startLevel)
    {
        return firstLevelUpLimit;
    }
    else
    {
        int32_t diff = currentLevel - startLevel;
        return firstLevelUpLimit + diff * 10;
    }
}

/**
 * @brief Implements the algorithm for the game phase START
 * 
 * @param game - a pointer to GameState that stores the current information about the game
 * @param input - a pointer to InputState that holds the current input from the user 
 */
void update_game_start(GameState *game, const InputState* input){
    if(input->deltaUp > 0){
        game->startLevel++;
    }

    if(input->deltaDown > 0 && game->startLevel > 0){
        game->startLevel--;
    }

    if(input->deltaA > 0){
        // Reset the game state and set the game phase to PLAY
        memset(game->board, 0, WIDTH * HEIGHT);
        game->level = game->startLevel;
        game->score = 0;
        game->lineCount = 0;
        spawn_piece(game);
        game->phase = GAME_PHASE_PLAY;
    }
}

/**
 * @brief Implements the algorithm for the game phase - GAME_PHASE_GAMEOVER
 * 
 * @param game - a pointer to GameState that stores the current information about the game
 * @param input - a pointer to InputState that holds the current input from the user
 */
void update_game_gameover(GameState *game, const InputState* input){
    if(input->deltaA > 0){
        game->phase = GAME_PHASE_START;
    }
}

/**
 * @brief Implements the algorithm for the game phase - GAME_PHASE_LINE
 *
 * @param game - a pointer to GameState that stores the current information about the game
 */
void update_game_line(GameState *game)
{
    if (game->time >= game->highlightEndTime)
    {
        clear_lines(game->board, WIDTH, HEIGHT, game->lines);

        game->lineCount += game->pendingLineCount;
        game->score += compute_score(game->level, game->pendingLineCount);

        // Getting the number of lines required for next level
        int32_t linesForNextLevel = get_lines_for_next_level(game->startLevel, game->level);
        if (game->lineCount >= linesForNextLevel)
        {
            game->level++;
        }

        game->phase = GAME_PHASE_PLAY;
    }
}

/**
 * @brief - Checks whether the row is empty.
 *
 * @param values - array containing board data
 * @param width - width of the board
 * @param row - number of rows in the board
 * @return uint8_t - 1 indicates filled while 0 indicates empty
 */
inline uint8_t check_row_empty(const uint8_t *values, int32_t width, int32_t row)
{
    for (int32_t col = 0; col < width; col++)
    {
        if (matrix_get(values, width, row, col))
        {
            return 0;
        }
    }
    return 1;
}

/**
 * @brief Implements the algorithm for the game phase - GAME_PHASE_PLAY. Updates the game state by validating the game moves and then, setting them on the board
 *
 * @param game - Pointer to GameState that holds the current state of the game
 * @param input - Pointer to InputState that holds the current input from the user
 */
void update_game_play(GameState *game, const InputState *input)
{
    PieceState piece = game->piece;

    // Processing key press
    if (input->deltaLeft > 0)
    {
        piece.offsetCol--;
    }
    if (input->deltaRight > 0)
    {
        piece.offsetCol++;
    }
    if (input->deltaUp > 0)
    {
        piece.rotation = (piece.rotation + 1) % 4;
    }

    // Copy valid piece into the game to update the game's state
    if (check_piece_valid(&piece, game->board, WIDTH, HEIGHT))
    {
        game->piece = piece;
    }

    // Soft drop every time down arrow key is pressed
    if (input->deltaDown > 0)
    {
        soft_drop(game);
    }

    // Hard drop
    if (input->deltaA > 0)
    {
        while (soft_drop(game))
            ;
    }

    // Soft drop
    while (game->time >= game->nextDropTime)
    {
        soft_drop(game);
    }

    game->pendingLineCount = find_lines(game->board, WIDTH, HEIGHT, game->lines);
    if (game->pendingLineCount > 0)
    {
        game->phase = GAME_PHASE_LINE;
        game->highlightEndTime = game->time + 0.5; // highlight end time is 500 ms
    }

    // Game over when tetrinos are in the two hidden rows at the top of the board
    int32_t gameOverRow = 0;
    if (!check_row_empty(game->board, WIDTH, gameOverRow))
    {
        game->phase = GAME_PHASE_GAMEOVER;
    }
}

/**
 * @brief - Updates the game's state based on the input received from the user
 *
 * @param game - Pointer to GameState holding information about the current state of the game
 * @param input - Pointer to InputState holding information about the last input recieved from the user
 */
void update_game(GameState *game, const InputState *input)
{
    switch (game->phase)
    {
    case GAME_PHASE_START:
        update_game_start(game, input);
        break;
    case GAME_PHASE_PLAY:
        update_game_play(game, input);
        break;
    case GAME_PHASE_LINE:
        update_game_line(game);
        break;
    case GAME_PHASE_GAMEOVER:
        update_game_gameover(game, input);
        break;
    }
}