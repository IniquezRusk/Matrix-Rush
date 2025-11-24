// g++ main.cpp player.cpp enemy.cpp displayProblem.cpp feedback.cpp -Ix86_64-w64-mingw32/include -Lx86_64-w64-mingw32/lib -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lSDL2_ttf -o main.exe

#define SDL_MAIN_HANDLED
#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <vector>
#include <cmath>
#include <ctime>
#include "player.h"
#include "enemy.h"
#include "feedback.h"
#include "displayProblem.h"

const int SCREEN_WIDTH = 1280;
const int SCREEN_HEIGHT = 720;

enum GameState {
    TITLE,
    PLAYING,
    PAUSED,
    GAME_OVER
};

struct Box {
    SDL_Rect r;
    int speed;
    SDL_Color col;
};

const int NUM_STRIPS = 5;

// Texture variables for scrolling background (globals)
SDL_Texture* bgTexture = nullptr;
int bgTexW = 0, bgTexH = 0;
int bgScaledH = 0;
float bgOffsetY = 0.0f;
float bgScrollSpeed = 300.0f; // pixels per second (tune this)
float bgScale = 1.0f;

int main() {
    SDL_Window* window = nullptr;
    std::string winner = "";

    // Seeds the random number generator so the questions are always completely random
    std::srand(static_cast<unsigned int>(std::time(nullptr)));

    // Error codes in case something does not render

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL could not be initialized! SDL Error: %s\n", SDL_GetError());
        return 1;
    }

    // Initialize IMG for JPG+PNG support (needed for .jpg files)
    if (!(IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG) & (IMG_INIT_JPG | IMG_INIT_PNG))) {
        SDL_Log("SDL_image could not initialize JPG/PNG: %s", IMG_GetError());
        // Not returning here — you may still want to continue for debug, but texture loads will fail.
    }

    if (TTF_Init() == -1) {
        printf("SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError());
        // continue or return depending on whether you want TTF required
    }

    window = SDL_CreateWindow("Matrix Rush", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);

    if (!window) {
        printf("Window could not be created! SDL Error:%s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        TTF_Quit(); IMG_Quit(); SDL_Quit();
        return 1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer) {
        std::printf("Renderer create error: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        TTF_Quit(); IMG_Quit(); SDL_Quit(); return 1;
    }

    TTF_Font* font = TTF_OpenFont("assets/yourfont.ttf", 24);
    if (!font) {
        std::cout << "Failed to load font! TTF_Error: " << TTF_GetError() << "\n";
        // continue or return depending on whether font is required
        return 1;
    }

    // ---- Load background ONCE using the GLOBAL bgTexture (do NOT redeclare a local bgTexture) ----
    bgTexture = IMG_LoadTexture(renderer, "assets/background.jpg"); // adjust path if needed
    if (!bgTexture) {
        SDL_Log("Failed to load bg (assets/background.jpg): %s", IMG_GetError());
        // bgTexture remains nullptr and background won't draw, but program continues
    } else {
        SDL_QueryTexture(bgTexture, NULL, NULL, &bgTexW, &bgTexH);
        // scale to screen width keeping aspect ratio
        bgScale = (float)SCREEN_WIDTH / (float)bgTexW;
        bgScaledH = (int)(bgTexH * bgScale);
        bgOffsetY = 0.0f;
        SDL_Log("BG loaded: w=%d h=%d scaledH=%d", bgTexW, bgTexH, bgScaledH);
    }
    // --------------------------------------------------------------------------------------------

    // Winning conditions
    int correctCount = 0;
    int incorrectCount = 0;

    // Variables for the boxes 
    int stripH = SCREEN_HEIGHT / 5;
    int colW = SCREEN_WIDTH / 6;
    int leftX = 0;
    int rightX = SCREEN_WIDTH - colW;

    // Makes the boxes go from light to dark
    SDL_Color shades[NUM_STRIPS] = {
        {255,255,255,255},
        {210,210,210,210},
        {160,160,160,160},
        {110,110,110,110},
        {60,60,60,255}
    };

    // Making two columns 5 boxes each
    Box leftCol[NUM_STRIPS];
    Box rightCol[NUM_STRIPS];

    for (int i = 0; i < NUM_STRIPS; i++) {
        int y = SCREEN_HEIGHT - (i+1)*stripH;

        leftCol[i].r = { leftX, y, colW, stripH};
        rightCol[i].r = { rightX, y, colW, stripH};

        leftCol[i].col = shades[i];
        rightCol[i].col = shades[i];

        // Giving them slightly different speeds so that they drift
        int base = 90;
        leftCol[i].speed = base + i*6;
        rightCol[i].speed = base + i*6;
    }

    // Initializing the player and enemy squares
    Player player;
    Enemy enemy;

    int squareW = colW * 0.6;
    int squareH = 70;

    // Centering the horizontally in the column
    int marginX = (colW - squareW) / 2;

    // Bottom positon (lowest strip)
    int bottomY = SCREEN_HEIGHT - stripH + (stripH - squareH) / 2;

    // Initializeing x, y, w, h, and step
    player.init(leftX + marginX, bottomY, squareW, squareH, stripH);
    enemy.init(rightX + marginX, bottomY, squareW, squareH, stripH);

    bool running = true;
    SDL_Event e;

    // Initializes the dt clock once
    static Uint32 prev = SDL_GetTicks();

    // Generates a matrix problem
    MatrixQuestion q;
    generateSystem(q, 3);

    // Timer for the game
    const float TIME_LIMIT = 180.0f;
    float timeLeft = TIME_LIMIT;

    // Used for text to font inputs
    std::string userInput[3] = {"", "", ""};
    int activeInput = 0;

    // Generating the feedback object
    Feedback feedback;

    auto drawColumn = [&](Box boxes[NUM_STRIPS]) {
        for (int i = 0; i < NUM_STRIPS; i++) {
            SDL_SetRenderDrawColor(renderer, boxes[i].col.r, boxes[i].col.g, boxes[i].col.b, 255);
            SDL_RenderFillRect(renderer, &boxes[i].r);
        }
    };

    GameState state = TITLE;

    SDL_StartTextInput();

    while (running) {
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                running = false;
            }

            // Start the game from title
            else if (state == TITLE && e.type == SDL_KEYDOWN) {
                state = PLAYING;
            }

            // Only handle gameplay input if actually playing
            else if (state == PLAYING && e.type == SDL_KEYDOWN) {
                switch (e.key.keysym.sym) {
                    case SDLK_ESCAPE:
                        running = false;
                        break;

                    case SDLK_TAB:
                        activeInput = (activeInput + 1) % 3;
                        break;

                    case SDLK_BACKSPACE:
                        if (!userInput[activeInput].empty())
                            userInput[activeInput].pop_back(); 
                        break;

                    case SDLK_RETURN:
                        try {
                            double x = std::stod(userInput[0]);
                            double y = std::stod(userInput[1]);
                            double z = std::stod(userInput[2]);

                            if (checkAnswer(q, x, y, z)) {
                                feedback.showCorrect();
                                player.markCorrect();
                                player.startMoveUp(1, 0.5f);

                                correctCount++;
                            } else { 
                                feedback.showIncorrect();
                                player.markIncorrect();
                                enemy.startMoveUp(1, 0.5f);

                                incorrectCount++;
                            }

                            if (correctCount >= 5) {
                                winner = "PLAYER";
                                state = GAME_OVER;
                            }

                            if (incorrectCount >= 5) {
                                winner = "ENEMY";
                                state = GAME_OVER;
                            }

                            if (state == PLAYING) {
                                userInput[0].clear();
                                userInput[1].clear();
                                userInput[2].clear();
                                generateSystem(q, 3);
                                timeLeft = TIME_LIMIT;
                            }

                        } catch (...) {
                            std::cout << "Invalid number\n";
                        }
                        break;
                }
            }

            // removed background update/render that was incorrectly inside the event loop

            else if (state == GAME_OVER && e.type == SDL_KEYDOWN) {
                // Resets the win counters
                correctCount = 0;
                incorrectCount = 0;
                winner = "";

                // Resets the enemy/player positions
                player.resetPos();
                enemy.resetPos();

                // Resets the input boxes
                userInput[0].clear();
                userInput[1].clear();
                userInput[2].clear();

                // Resets the timer
                timeLeft = TIME_LIMIT;

                // Generate new matrix problem
                generateSystem(q, 3);

                // Back to gameplay
                state = PLAYING;
            }

            // Only accept typing when PLAYING
            else if (state == PLAYING && e.type == SDL_TEXTINPUT) {
                userInput[activeInput] += e.text.text;
            }
        } // end event polling

        // --- compute dt BEFORE updating/rendering per-frame things ---
        Uint32 now = SDL_GetTicks();
        float dt = (now - prev) / 1000.0f;
        if (dt > 0.1f) dt = 0.1f;
        prev = now;

        // CLEAR SCREEN
        SDL_SetRenderDrawColor(renderer, 12, 14, 20, 255);
        SDL_RenderClear(renderer);

        // --- background update & render (only when playing) ---
// --- background update & render (only when playing) ---
if (bgTexture && state == PLAYING) {
    // scroll UP
    bgOffsetY -= bgScrollSpeed * dt;

    // wrap when fully scrolled upwards
    if (bgOffsetY <= -bgScaledH)
        bgOffsetY += bgScaledH;

    // draw enough tiles to cover screen vertically
    if (bgScaledH <= 0) {
        SDL_Rect r = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
        SDL_RenderCopy(renderer, bgTexture, NULL, &r);
    } else {
        int tilesNeeded = (SCREEN_HEIGHT / bgScaledH) + 2; // extra tile for smooth wrapping
        float startY = bgOffsetY; // topmost tile
        for (int i = 0; i < tilesNeeded; ++i) {
            SDL_Rect dst = {0, (int)(startY + i * bgScaledH), SCREEN_WIDTH, bgScaledH};
            SDL_RenderCopy(renderer, bgTexture, NULL, &dst);
        }
    }
}



        // --- end background ---

        if (state == TITLE) {

            SDL_SetRenderDrawColor(renderer, 0, 0, 20, 255);
            SDL_RenderClear(renderer);

            SDL_Color green = {0,255,0};

            // ----- TITLE TEXT -----
            SDL_Surface* titleSurface = TTF_RenderText_Solid(font, "[MATRIX RUSH]", green);
            SDL_Texture* titleTex = SDL_CreateTextureFromSurface(renderer, titleSurface);

            SDL_Rect titleRect = {
                SCREEN_WIDTH / 2 - titleSurface->w / 2,
                150,
                titleSurface->w,
                titleSurface->h
            };

            SDL_RenderCopy(renderer, titleTex, NULL, &titleRect);
            SDL_FreeSurface(titleSurface);
            SDL_DestroyTexture(titleTex);

            // ----- PRESS ANY KEY -----
            if ((SDL_GetTicks() / 500) % 2 == 0) {
                SDL_Surface* msgSurf = TTF_RenderText_Solid(font, "PRESS ANY KEY TO START", green);
                SDL_Texture* msgTex = SDL_CreateTextureFromSurface(renderer, msgSurf);

                SDL_Rect msgRect = {
                    SCREEN_WIDTH / 2 - msgSurf->w / 2,
                    SCREEN_HEIGHT - 120,
                    msgSurf->w,
                    msgSurf->h
                };

                SDL_RenderCopy(renderer, msgTex, NULL, &msgRect);
                SDL_FreeSurface(msgSurf);
                SDL_DestroyTexture(msgTex);
            }

            SDL_RenderPresent(renderer);
            continue;
        }

        if (state == GAME_OVER) {
            SDL_SetRenderDrawColor(renderer, 0, 0, 20, 255);
            SDL_RenderClear(renderer);

            SDL_Color red   = {255, 0, 0};
            SDL_Color green = {0, 255, 0};

            // GAME OVER
            SDL_Surface* goSurf = TTF_RenderText_Solid(font, "[GAME OVER]", red);
            SDL_Texture* goTex = SDL_CreateTextureFromSurface(renderer, goSurf);

            SDL_Rect goRect = {
                SCREEN_WIDTH/2 - goSurf->w/2,
                SCREEN_HEIGHT/2 - 50,
                goSurf->w,
                goSurf->h
            };

            SDL_RenderCopy(renderer, goTex, NULL, &goRect);
            SDL_FreeSurface(goSurf);
            SDL_DestroyTexture(goTex);

            // WIN/LOSE message 
            SDL_Surface* resultSurf = nullptr;
            SDL_Texture* resultTex = nullptr;

            if (winner == "PLAYER") {
                resultSurf = TTF_RenderText_Solid(font, "[YOU WIN]", green);
            } else {
                resultSurf = TTF_RenderText_Solid(font, "[YOU LOSE]", red);
            }

            // Blinking press R to restart
            if ((SDL_GetTicks() / 500) % 2 == 0) {
                SDL_Color white = {255, 255, 255};

                SDL_Surface* restartSurf = TTF_RenderText_Solid(font, "Press R to Restart", white);
                SDL_Texture* restartTex = SDL_CreateTextureFromSurface(renderer, restartSurf);

                SDL_Rect restartRect = {
                    SCREEN_WIDTH/2 - restartSurf->w/2,
                    SCREEN_HEIGHT/2 + 90,
                    restartSurf->w,
                    restartSurf->h
                };

                SDL_RenderCopy(renderer, restartTex, NULL, &restartRect);

                SDL_FreeSurface(restartSurf);
                SDL_DestroyTexture(restartTex);
            }

            resultTex = SDL_CreateTextureFromSurface(renderer, resultSurf);

            SDL_Rect resRect = {
                SCREEN_WIDTH/2 - resultSurf->w/2,
                SCREEN_HEIGHT/2 + 20,
                resultSurf->w,
                resultSurf->h
            };

            SDL_RenderCopy(renderer, resultTex, NULL, &resRect);

            SDL_FreeSurface(resultSurf);
            SDL_DestroyTexture(resultTex);

            SDL_RenderPresent(renderer);
            continue;
        }

        player.update(dt);
        enemy.update(dt);
        feedback.update(dt);

        timeLeft -= dt;
        if (timeLeft <= 0.0f) {
            player.markTimeOut();
            enemy.startMoveUp(1, 0.5f);
            userInput[0].clear();
            userInput[1].clear();
            userInput[2].clear();
            generateSystem(q, 3);
            timeLeft = TIME_LIMIT;
        }

        // Timer
        int seconds = std::ceil(timeLeft);
        if (seconds < 0) seconds = 0;

        char buf[16];
        snprintf(buf, sizeof(buf), "%02d:%02d", seconds/60, seconds%60);
        std::string timeStr = buf;

        int textW, textH;
        TTF_SizeText(font, timeStr.c_str(), &textW, &textH);

        renderText(renderer, font, timeStr,
                   SCREEN_WIDTH/2 - textW/2, 20,
                   {255,255,255,255});

        // Columns, player, enemy
        drawColumn(leftCol);
        drawColumn(rightCol);
        player.render(renderer);
        enemy.render(renderer);

        // Matrix problem
        renderMatrixQuestion(renderer, font, q, {255,255,255,255});

        // Input boxes
        int spacing = 60;
        int startX = SCREEN_WIDTH / 2;
        int startY = (SCREEN_HEIGHT - 3*spacing)/2 + q.size*50;

        std::string labels[3] = {"x = ", "y = ", "z = "};
        for (int i = 0; i < 3; i++) {
            TTF_SizeText(font, labels[i].c_str(), &textW, &textH);
            renderText(renderer, font,
                       labels[i] + userInput[i],
                       startX - textW/2,
                       startY + i*spacing,
                       {255,255,255,255});
        }

        feedback.render(renderer, font, q);

        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    } // end main loop

    SDL_StopTextInput();

    // cleanup bgTexture once at shutdown
    if (bgTexture) {
        SDL_DestroyTexture(bgTexture);
        bgTexture = nullptr;
    }

    // cleanup SDL stuff
    if (font) TTF_CloseFont(font);
    if (renderer) SDL_DestroyRenderer(renderer);
    if (window) SDL_DestroyWindow(window);
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();

    return 0;
}

SDL_StopTextInput();
}

