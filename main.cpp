// g++ main.cpp player.cpp enemy.cpp displayProblem.cpp -Ix86_64-w64-mingw32/include -Lx86_64-w64-mingw32/lib -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lSDL2_ttf -o main.exe


#define SDL_MAIN_HANDLED
#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <vector>
#include "player.h"
#include "enemy.h"
#include "displayProblem.h"

const int SCREEN_WIDTH = 1280;
const int SCREEN_HEIGHT = 720;

struct Box {
    SDL_Rect r;
    int speed;
    SDL_Color col;
};

const int NUM_STRIPS = 5;

int main() {
    SDL_Window* window = nullptr;

    // Error codes in case something does not render

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL_image could not be initialized! SDL_Image Error: \n", IMG_GetError());
        return 1;
    }

    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        printf("SDL_image could not initialize! SDL_Image Error: %s\n", IMG_GetError());
    }

    if (TTF_Init() == -1) {
        printf("SDL_ttf could not initialize! SDL_ttf Error:L %s\n", IMG_GetError());
    }

    window = SDL_CreateWindow("Matrix Rush", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);

    if (!window) {
        printf("Renderer could not be created! SDL Error:%s\n", SDL_GetError());
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
    return 1;
}


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

    int bgX = 0;
    bool running = true;
    SDL_Event e;

    // Initializes the dt clock once
    static Uint32 prev = SDL_GetTicks();

    // Generates a matrix problem
    MatrixQuestion q;
    generateSystem(q, 3);

    // Used for text to font inputs
    std::string userInput[3] = {"", "", ""};
    int activeInput = 0;

    auto drawColumn = [&](Box boxes[NUM_STRIPS]) {
        for (int i = 0; i < NUM_STRIPS; i++) {
            SDL_SetRenderDrawColor(renderer, boxes[i].col.r, boxes[i].col.g, boxes[i].col.b, 255);
            SDL_RenderFillRect(renderer, &boxes[i].r);
        }
    };

SDL_StartTextInput();
while (running) {
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_QUIT) 
        running = false;
        else if (e.type == SDL_KEYDOWN) {
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

                        if (checkAnswer(q, x, y, z))
                            std::cout << "Correct!\n";
                        else 
                            std::cout << "Incorrect!\n";
                     } catch (std::invalid_argument&) {
                        std::cout << "Please enter a valid number for all inputs!\n";
                    } catch (std::out_of_range&) {
                        std::cout << "Input number is too large!\n";
                    }
                    break;
            }
        }
        else if (e.type == SDL_TEXTINPUT) {
            userInput[activeInput] += e.text.text;
        }
    }


    // Render
    SDL_SetRenderDrawColor(renderer, 12, 14, 20, 255);
    SDL_RenderClear(renderer);

    // Columns, player, enemy, etc.
    drawColumn(leftCol);
    drawColumn(rightCol);
    player.render(renderer);
    enemy.render(renderer);

    SDL_Color White = {255,255,255,255};

    // Draw matrix equations
    renderMatrixQuestion(renderer, font, q, White);

    // Draw solution lines
    int spacing = 60;
    int startX = SCREEN_WIDTH / 2;
    int startY = (SCREEN_HEIGHT - 3 * spacing)/2 + q.size*50; // below equations
    std::string labels[3] = {"x = ", "y = ", "z = "};
    for (int i = 0; i < 3; i++) {
        int textW, textH;
        TTF_SizeText(font, labels[i].c_str(), &textW, &textH);
        renderText(renderer, font, labels[i] + userInput[i], startX - textW/2, startY + i*spacing, White);
    }

    SDL_RenderPresent(renderer);
}
SDL_StopTextInput();


        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        TTF_Quit();
        IMG_Quit();
        SDL_Quit();

        return 0;
}
