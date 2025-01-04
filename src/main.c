#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL_error.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>

#define SCREEN_WIDTH 510
#define SCREEN_HEIGHT 510
#define GRID_SIZE 4
#define TILE_SIZE 120
#define PADDING 10

typedef struct {
    int value;
    bool merged;
} Tile;

Tile grid[GRID_SIZE][GRID_SIZE];
int score = 0;
bool game_over = false;
bool win = false;

SDL_Color getColorForValue(int value) {
    switch(value) {
        case 2: return (SDL_Color){238, 228, 218, 255};
        case 4: return (SDL_Color){237, 224, 200, 255};
        case 8: return (SDL_Color){242, 177, 121, 255};
        case 16: return (SDL_Color){245, 149, 99, 255};
        case 32: return (SDL_Color){246, 124, 95, 255};
        case 64: return (SDL_Color){246, 94, 59, 255};
        case 128: return (SDL_Color){237, 207, 114, 255};
        case 256: return (SDL_Color){237, 204, 97, 255};
        case 512: return (SDL_Color){237, 200, 80, 255};
        case 1024: return (SDL_Color){237, 197, 63, 255};
        case 2048: return (SDL_Color){237, 194, 46, 255};
        default: return (SDL_Color){205, 193, 180, 255};
    }
}

void resetMergedStatus() {
    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            grid[i][j].merged = false;
        }
    }
}

void addRandomTile() {
    int empty_cells[GRID_SIZE * GRID_SIZE][2];
    int count = 0;

    // Find empty cells
    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            if (grid[i][j].value == 0) {
                empty_cells[count][0] = i;
                empty_cells[count][1] = j;
                count++;
            }
        }
    }

    if (count > 0) {
        int index = rand() % count;
        int x = empty_cells[index][0];
        int y = empty_cells[index][1];

        // 90% chance of 2, 10% chance of 4
        grid[x][y].value = (rand() % 10 == 0) ? 4 : 2;
    }
}

void initializeGame() {
    srand(time(NULL));
    memset(grid, 0, sizeof(grid));
    score = 0;
    game_over = false;
    win = false;

    // Add two initial tiles
    addRandomTile();
    addRandomTile();
}

bool canMove() {
    // Check if any move is possible
    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            // Check for empty cells
            if (grid[i][j].value == 0) return true;

            // Check horizontal neighbors
            if (j < GRID_SIZE - 1 && grid[i][j].value == grid[i][j+1].value)
                return true;

            // Check vertical neighbors
            if (i < GRID_SIZE - 1 && grid[i][j].value == grid[i+1][j].value)
                return true;
        }
    }
    return false;
}

void moveLeft() {
    bool moved = false;
    resetMergedStatus();

    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 1; j < GRID_SIZE; j++) {
            if (grid[i][j].value == 0) continue;

            int k = j;
            while (k > 0 && grid[i][k-1].value == 0) {
                grid[i][k-1].value = grid[i][k].value;
                grid[i][k].value = 0;
                k--;
                moved = true;
            }

            // Merge tiles
            if (k > 0 && grid[i][k-1].value == grid[i][k].value &&
                !grid[i][k-1].merged && !grid[i][k].merged) {
                grid[i][k-1].value *= 2;
                score += grid[i][k-1].value;
                grid[i][k-1].merged = true;
                grid[i][k].value = 0;
                moved = true;

                // Check for win condition
                if (grid[i][k-1].value == 2048) win = true;
            }
        }
    }

    if (moved) addRandomTile();
}

void moveRight() {
    bool moved = false;
    resetMergedStatus();

    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = GRID_SIZE - 2; j >= 0; j--) {
            if (grid[i][j].value == 0) continue;

            int k = j;
            while (k < GRID_SIZE - 1 && grid[i][k+1].value == 0) {
                grid[i][k+1].value = grid[i][k].value;
                grid[i][k].value = 0;
                k++;
                moved = true;
            }

            // Merge tiles
            if (k < GRID_SIZE - 1 && grid[i][k+1].value == grid[i][k].value &&
                !grid[i][k+1].merged && !grid[i][k].merged) {
                grid[i][k+1].value *= 2;
                score += grid[i][k+1].value;
                grid[i][k+1].merged = true;
                grid[i][k].value = 0;
                moved = true;

                // Check for win condition
                if (grid[i][k+1].value == 2048) win = true;
            }
        }
    }

    if (moved) addRandomTile();
}

void moveUp() {
    bool moved = false;
    resetMergedStatus();

    for (int j = 0; j < GRID_SIZE; j++) {
        for (int i = 1; i < GRID_SIZE; i++) {
            if (grid[i][j].value == 0) continue;

            int k = i;
            while (k > 0 && grid[k-1][j].value == 0) {
                grid[k-1][j].value = grid[k][j].value;
                grid[k][j].value = 0;
                k--;
                moved = true;
            }

            // Merge tiles
            if (k > 0 && grid[k-1][j].value == grid[k][j].value &&
                !grid[k-1][j].merged && !grid[k][j].merged) {
                grid[k-1][j].value *= 2;
                score += grid[k-1][j].value;
                grid[k-1][j].merged = true;
                grid[k][j].value = 0;
                moved = true;

                // Check for win condition
                if (grid[k-1][j].value == 2048) win = true;
            }
        }
    }

    if (moved) addRandomTile();
}

void moveDown() {
    bool moved = false;
    resetMergedStatus();

    for (int j = 0; j < GRID_SIZE; j++) {
        for (int i = GRID_SIZE - 2; i >= 0; i--) {
            if (grid[i][j].value == 0) continue;

            int k = i;
            while (k < GRID_SIZE - 1 && grid[k+1][j].value == 0) {
                grid[k+1][j].value = grid[k][j].value;
                grid[k][j].value = 0;
                k++;
                moved = true;
            }

            // Merge tiles
            if (k < GRID_SIZE - 1 && grid[k+1][j].value == grid[k][j].value &&
                !grid[k+1][j].merged && !grid[k][j].merged) {
                grid[k+1][j].value *= 2;
                score += grid[k+1][j].value;
                grid[k+1][j].merged = true;
                grid[k][j].value = 0;
                moved = true;

                // Check for win condition
                if (grid[k+1][j].value == 2048) win = true;
            }
        }
    }

    if (moved) addRandomTile();
}

void drawGrid(SDL_Renderer* renderer, TTF_Font* font) {
    // Background
    SDL_SetRenderDrawColor(renderer, 187, 173, 160, 255);
    SDL_RenderClear(renderer);

    // Grid background
    SDL_SetRenderDrawColor(renderer, 205, 193, 180, 255);
    SDL_Rect gridRect = {
        PADDING,
        PADDING,
        SCREEN_WIDTH - 2 * PADDING,
        SCREEN_HEIGHT - 2 * PADDING
    };
    SDL_RenderFillRect(renderer, &gridRect);

    // Draw tiles
    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            SDL_Rect rect = {
                j * TILE_SIZE + PADDING * 2,
                i * TILE_SIZE + PADDING * 2,
                TILE_SIZE - PADDING,
                TILE_SIZE - PADDING
            };

            // Empty tile background
            SDL_SetRenderDrawColor(renderer, 238, 228, 218, 100);
            SDL_RenderFillRect(renderer, &rect);

            if (grid[i][j].value > 0) {
                SDL_Color tileColor = getColorForValue(grid[i][j].value);
                SDL_SetRenderDrawColor(renderer,
                    tileColor.r,
                    tileColor.g,
                    tileColor.b,
                    tileColor.a
                );
                SDL_RenderFillRect(renderer, &rect);

                // Render number
                char text[10];
                sprintf(text, "%d", grid[i][j].value);
                SDL_Color textColor = {0, 0, 0, 255};
                SDL_Surface* surface = TTF_RenderText_Blended(font, text, textColor);
                SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);

                int text_w = surface->w;
                int text_h = surface->h;
                SDL_Rect textRect = {
                    j * TILE_SIZE + (TILE_SIZE - text_w)/2 + PADDING * 2,
                    i * TILE_SIZE + (TILE_SIZE - text_h)/2 + PADDING * 2,
                    text_w,
                    text_h
                };

                SDL_RenderCopy(renderer, texture, NULL, &textRect);
                SDL_FreeSurface(surface);
                SDL_DestroyTexture(texture);
            }
        }
    }

    // Render score
    char scoreText[50];
    sprintf(scoreText, "Score: %d", score);
    SDL_Color scoreColor = {255, 255, 255, 255};
    SDL_Surface* scoreSurface = TTF_RenderText_Blended(font, scoreText, scoreColor);
    SDL_Texture* scoreTexture = SDL_CreateTextureFromSurface(renderer, scoreSurface);

    SDL_Rect scoreRect = {10, 10, scoreSurface->w, scoreSurface->h};
    SDL_RenderCopy(renderer, scoreTexture, NULL, &scoreRect);

    SDL_FreeSurface(scoreSurface);
    SDL_DestroyTexture(scoreTexture);

    // Game over or win screen
    if (game_over || win) {
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 128);
        SDL_Rect overlayRect = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
        SDL_RenderFillRect(renderer, &overlayRect);

        char endText[11];
        sprintf(endText, win ? "You Win! Score: %d" : "Game Over! Score: %d", score);
        SDL_Surface* endSurface = TTF_RenderText_Blended(font, endText, (SDL_Color){255,255,255,255});
        SDL_Texture* endTexture = SDL_CreateTextureFromSurface(renderer, endSurface);

        SDL_Rect endRect = {
            (SCREEN_WIDTH - endSurface->w) / 2,
            (SCREEN_HEIGHT - endSurface->h) / 2,
            endSurface->w,
            endSurface->h
        };
        SDL_RenderCopy(renderer, endTexture, NULL, &endRect);

        SDL_FreeSurface(endSurface);
        SDL_DestroyTexture(endTexture);
    }

    SDL_RenderPresent(renderer);
}

int main() {
    SDL_Init(SDL_INIT_VIDEO);
    TTF_Init();

    SDL_Window* window = SDL_CreateWindow(
        "2048",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        SCREEN_WIDTH,
        SCREEN_HEIGHT,
        0
    );

    if (window == NULL) {
        fprintf(stderr, "SDL_CreateWindow() failed: %s", SDL_GetError());
        return -1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == NULL) {
        fprintf(stderr, "SDL_CreateRenderer() failed: %s", SDL_GetError());
        return -1;
    }

    if (fopen("font.ttf", "r") == NULL) {
        fprintf(stderr, "Failed to open font!");
        return -1;
    }
    TTF_Font* font = TTF_OpenFont("font.ttf", 32);

    initializeGame();

    bool quit = false;
    SDL_Event event;

    while (!quit) {
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    quit = true;
                    break;
                case SDL_KEYDOWN:
                    if (!game_over && !win) {
                        switch (event.key.keysym.sym) {
                            case SDLK_LEFT: moveLeft(); break;
                            case SDLK_RIGHT: moveRight(); break;
                            case SDLK_UP: moveUp(); break;
                            case SDLK_DOWN: moveDown(); break;
                            case SDLK_r: initializeGame(); break;
                        }
                    }

                    // Restart game
                    if (game_over || win) {
                        if (event.key.keysym.sym == SDLK_r) {
                            initializeGame();
                        }
                    }
                    break;
            }
        }

        // Check game over condition
        if (!canMove()) {
            game_over = true;
        }
        drawGrid(renderer, font);
        SDL_Delay(16);  // Cap framerate
    }

    // Cleanup
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_CloseFont(font);
    SDL_Quit();
    TTF_Quit();

    return 0;
}
