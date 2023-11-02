#include <cassert>
#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "./inc/tetris.h"

// Function Prototypes
void fill_rect(SDL_Renderer *renderer, int32_t x, int32_t y, int32_t width, int32_t height, Color color);
void draw_string(SDL_Renderer *renderer, TTF_Font *font, const char *text, int32_t x, int32_t y, TextAlign alignment, Color color);
void draw_cell(SDL_Renderer *renderer, int32_t row, int32_t col, uint8_t colorValue, int32_t xOffset, int32_t yOffset, bool outline);
void draw_piece(SDL_Renderer *renderer, const PieceState *piece, int32_t xOffset, int32_t yOffset, bool outline);
void render_game(const GameState *game, SDL_Renderer *renderer, TTF_Font *font);

/**
 * @brief Creates and fills the rectangle properties with the received parameters
 *
 * @param renderer - a pointer to SDL_Renderer* that renders the created rectangle
 * @param x - x coordinate of the rectangle
 * @param y - y coordinate of the rectangle
 * @param width - width of the rectangle
 * @param height - height of the rectangle
 * @param color - contains the colors to be filled
 */
void fill_rect(SDL_Renderer *renderer, int32_t x, int32_t y, int32_t width, int32_t height, Color color)
{
    SDL_Rect rect = {};
    rect.x = x;
    rect.y = y;
    rect.w = width;
    rect.h = height;
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    SDL_RenderFillRect(renderer, &rect);
}

/**
 * @brief Draws an outline of the rectangle with the received parameters as its properties
 *
 * @param renderer - a pointer to SDL_Renderer* that renders the created rectangle
 * @param x - x coordinate of the rectangle
 * @param y - y coordinate of the rectangle
 * @param width - width of the rectangle
 * @param height - height of the rectangle
 * @param color - contains the colors to be filled
 */
void draw_rect(SDL_Renderer *renderer, int32_t x, int32_t y, int32_t width, int32_t height, Color color)
{
    SDL_Rect rect = {};
    rect.x = x;
    rect.y = y;
    rect.w = width;
    rect.h = height;
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    SDL_RenderDrawRect(renderer, &rect);
}

/**
 * @brief - Renders the text with a particular font style on the board
 *
 * @param renderer - a pointer to SDL_Renderer* that renders the font
 * @param font - font of the text
 * @param text - text to displayed
 * @param x - x coordinate of the rectangle on which the text is displayed
 * @param y - y coordinate of the rectangle on which the text is displayed
 * @param alignment - alignment of the text on the board
 * @param color - color of the text
 */
void draw_string(SDL_Renderer *renderer, TTF_Font *font, const char *text, int32_t x, int32_t y, TextAlign alignment, Color color)
{
    SDL_Color sdlColor = SDL_Color{color.r, color.g, color.b, color.a};
    SDL_Surface *surface = TTF_RenderText_Solid(font, text, sdlColor);
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);

    SDL_Rect rect;
    rect.w = surface->w;
    rect.h = surface->h;
    rect.y = y;

    switch (alignment)
    {
    case TEXT_ALIGN_LEFT:
        rect.x = x;
        break;
    case TEXT_ALIGN_CENTER:
        rect.x = x - surface->w / 2;
        break;
    case TEXT_ALIGN_RIGHT:;
        rect.x = x - surface->w;
        break;
    }

    SDL_RenderCopy(renderer, texture, 0, &rect);
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}

/**
 * @brief - Creates the cell (i.e. any object that is) to be drawn on the SDL window
 *
 * @param renderer - a pointer to SDL_Renderer* that renders the created cell
 * @param row - the row at which the cell is to be drawn
 * @param col - the column at which the cell is to be drawn
 * @param colorValue - index value for the various Color arrays
 * @param xOffset - x offset of the cell
 * @param yOffset - y offset of the cell
 * @param outline - if true, draws a silhouette of the tetrino piece on the board
 */
void draw_cell(SDL_Renderer *renderer, int32_t row, int32_t col, uint8_t colorValue, int32_t xOffset, int32_t yOffset, bool outline = false)
{

    Color baseColor = BASE_COLORS[colorValue];
    Color lightColor = LIGHT_COLORS[colorValue];
    Color darkColor = DARK_COLORS[colorValue];

    int32_t edge = GRID_SIZE / 8;

    int32_t x = col * GRID_SIZE + xOffset;
    int32_t y = row * GRID_SIZE + yOffset;

    // Drawing a silhoutte if outline is true
    if (outline)
    {
        draw_rect(renderer, x, y, GRID_SIZE, GRID_SIZE, baseColor);
        return;
    }

    // Filling the dark color first followed by light color and then by base color to generate nice affects
    fill_rect(renderer, x, y, GRID_SIZE, GRID_SIZE, darkColor);
    fill_rect(renderer, x + edge, y + edge, GRID_SIZE - edge, GRID_SIZE - edge, lightColor);
    fill_rect(renderer, x + edge, y + edge, GRID_SIZE - edge * 2, GRID_SIZE - edge * 2, baseColor);
}

/**
 * @brief - Draws the tetrino piece on the SDL window
 *
 * @param renderer - a pointer to SDL_Renderer* for renderering the piece
 * @param piece - piece to be rendered
 * @param xOffset - x offset of the piece when placed on the SDL window
 * @param yOffset - y offset of the piece when placed on the SDL window
 * @param outline - if true, draws a silhouette of the tetrino piece on the board
 */
void draw_piece(SDL_Renderer *renderer, const PieceState *piece, int32_t xOffset, int32_t yOffset, bool outline = false)
{
    const Tetrino *tetrino = TETRINOS + piece->tetrinoIndex;
    for (int32_t row = 0; row < tetrino->side; row++)
    {
        for (int32_t col = 0; col < tetrino->side; col++)
        {
            uint8_t value = tetrino_get(tetrino, row, col, piece->rotation);
            if (value)
            {
                draw_cell(renderer, row + piece->offsetRow, col + piece->offsetCol, value, xOffset, yOffset, outline);
            }
        }
    }
}

/**
 * @brief - Draws the game board on the SDL window
 *
 * @param renderer - a pointer to SDL_Renderer* for renderering the board
 * @param board - the board to be rendered
 * @param width - width of the board
 * @param height - height of the board
 * @param xOffset - x offset of the board when placed on the SDL window
 * @param yOffset - y offset of the board when placed on the SDL window
 */
void draw_board(SDL_Renderer *renderer, const uint8_t *board, int32_t width, int32_t height, int32_t xOffset, int32_t yOffset)
{
    for (int32_t row = 0; row < height; row++)
    {
        for (int32_t col = 0; col < width; col++)
        {
            uint8_t value = matrix_get(board, width, row, col);
            draw_cell(renderer, row, col, value, xOffset, yOffset);
            // if (value)
            // {
            //     draw_cell(renderer, row, col, value, xOffset, yOffset);
            // }
        }
    }
}

/**
 * @brief - Renders the game objects on the board
 *
 * @param game - pointer to GameState that contains the current state of the game
 * @param renderer - a pointer to SDL_Renderer* that renders the game object
 * @param font - a pointer to TTF_Font* holding the font type of the text to be rendererd
 */
void render_game(const GameState *game, SDL_Renderer *renderer, TTF_Font *font)
{

    int32_t paddingY = 60;

    draw_board(renderer, game->board, WIDTH, HEIGHT, 0, paddingY);
    if (game->phase == GAME_PHASE_PLAY)
    {
        draw_piece(renderer, &game->piece, 0, paddingY);

        PieceState piece = game->piece;
        while (check_piece_valid(&piece, game->board, WIDTH, HEIGHT))
        {
            piece.offsetRow++;
        }
        piece.offsetRow--;
    
        // Draw the silhouette of the piece
        draw_piece(renderer, &piece, 0, paddingY, true);
    }

    // Highlights the filled lines which will be cleared from the screen
    Color highlightColor = color(0xFF, 0xFF, 0xFF, 0xFF);
    if (game->phase == GAME_PHASE_LINE)
    {
        for (int32_t row = 0; row < HEIGHT; row++)
        {
            if (game->lines[row])
            {
                int32_t x = 0;
                int32_t y = row * GRID_SIZE + paddingY;

                fill_rect(renderer, x, y, WIDTH * GRID_SIZE, GRID_SIZE, highlightColor);
            }
        }
    }
    else if (game->phase == GAME_PHASE_GAMEOVER)
    {
        int32_t x = WIDTH * GRID_SIZE / 2;
        int32_t y = HEIGHT * GRID_SIZE / 2;
        draw_string(renderer, font, "GAME OVER", x, y, TEXT_ALIGN_CENTER, highlightColor);
    }
    else if (game->phase == GAME_PHASE_START)
    {
        int32_t x = WIDTH * GRID_SIZE / 2;
        int32_t y = HEIGHT * GRID_SIZE / 2;
        draw_string(renderer, font, "PRESS SPACE TO START", x, y, TEXT_ALIGN_CENTER, highlightColor);

        string buffer = "STARTING LEVEL: " + to_string(game->startLevel);
        draw_string(renderer, font, buffer.c_str(), x, y + 30, TEXT_ALIGN_CENTER, highlightColor);
    }

    fill_rect(renderer, 0, paddingY, WIDTH * GRID_SIZE, (HEIGHT - VISIBLE_HEIGHT) * GRID_SIZE, color(0x00, 0x00, 0x00, 0x00));

    // Display the level
    string buffer = "LEVEL: " + to_string(game->level);
    draw_string(renderer, font, buffer.c_str(), 5, 5, TEXT_ALIGN_LEFT, highlightColor);

    // Display the score
    buffer = "SCORE: " + to_string(game->score);
    draw_string(renderer, font, buffer.c_str(), 5, 35, TEXT_ALIGN_LEFT, highlightColor);

    // Display the line count
    buffer = "LINES: " + to_string(game->lineCount);
    draw_string(renderer, font, buffer.c_str(), 5, 65, TEXT_ALIGN_LEFT, highlightColor);
}

int main(void)
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        return 1;
    }

    if (TTF_Init() < 0)
    {
        return 2;
    }

    SDL_Window *window = SDL_CreateWindow("Tetris", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 420, 720, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    const char *fontName = "./chicken_pie/chicken_pie.ttf";
    TTF_Font *font = TTF_OpenFont(fontName, 16);

    GameState game = {};
    InputState input = {};

    spawn_piece(&game);

    game.piece.tetrinoIndex = 2;

    bool quit = false;
    while (!quit)
    {
        game.time = SDL_GetTicks() / 1000.0;
        SDL_Event e;
        while (SDL_PollEvent(&e) != 0)
        {
            if (e.type == SDL_QUIT)
            {
                quit = true;
            }
        }

        int32_t keyCount;
        const uint8_t *keyStates = SDL_GetKeyboardState(&keyCount);

        // Quit when escape key is pressed
        if (keyStates[SDL_SCANCODE_ESCAPE])
        {
            quit = true;
        }

        InputState prevInput = input;

        input.left = keyStates[SDL_SCANCODE_LEFT];
        input.right = keyStates[SDL_SCANCODE_RIGHT];
        input.up = keyStates[SDL_SCANCODE_UP];
        input.down = keyStates[SDL_SCANCODE_DOWN];
        input.a = keyStates[SDL_SCANCODE_SPACE];

        input.deltaLeft = input.left - prevInput.left;
        input.deltaRight = input.right - prevInput.right;
        input.deltaUp = input.up - prevInput.up;
        input.deltaDown = input.down - prevInput.down;
        input.deltaA = input.a - prevInput.a;

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
        SDL_RenderClear(renderer);

        update_game(&game, &input);
        render_game(&game, renderer, font);

        SDL_RenderPresent(renderer);
    }

    TTF_CloseFont(font);
    SDL_DestroyRenderer(renderer);
    SDL_Quit();

    return 0;
}