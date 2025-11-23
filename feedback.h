#ifndef FEEDBACK_H
#define FEEDBACK_H
#include "displayProblem.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>


class Feedback {
    public:
        Feedback();

        void showCorrect();
        void showIncorrect();

        void update(float dt);
        void render(SDL_Renderer* renderer, TTF_Font* font, const MatrixQuestion& q);

        bool isActive() const { return active; }

    private:
        bool active;
        bool correct;
        float timer;
        int blinkCount;
        float showSolutionTime;
        bool showingSolution;
};

#endif