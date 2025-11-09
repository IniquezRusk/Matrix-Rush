#ifndef DISPLAYPROBLEM_H
#define DISPLAYPROBLEM_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <string>
#include <cstdlib>

struct MatrixQuestion {
    int size; // Up to 4
    int A[4][4];
    int B[4][4];
    int correct[4][4];
};

void randomizeMatrix(MatrixQuestion& question, int n, int range = 3);

bool checkAnswer(const MatrixQuestion& q, const int ans[4][4]);

#endif
