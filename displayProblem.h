#ifndef DISPLAYPROBLEM_H
#define DISPLAYPROBLEM_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <string>
#include <cstdlib>

struct MatrixQuestion {
    int size; // Up to 4
    int A[3][3];
    int B[3];
    double solution[3];
};

bool solve(MatrixQuestion& q);

void generateSystem(MatrixQuestion& q, int n);

bool solve(MatrixQuestion& q);

bool checkAnswer(MatrixQuestion& q, double userX, double userY, double userZ);

// SDL2 Render text display
void renderText(SDL_Renderer* renderer, TTF_Font* font, const std::string& text, int x, int y, SDL_Color color);
void renderMatrixQuestion(SDL_Renderer* renderer, TTF_Font* font, const MatrixQuestion& q, SDL_Color color);
void renderSolutionLines(SDL_Renderer* renderer, TTF_Font* font, SDL_Color color);

#endif
