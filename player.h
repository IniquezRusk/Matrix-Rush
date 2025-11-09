/* The purpose of this file is for the player. It has 3 variables, running out of time,
answeing correctly, and answering incorrectly. If they are correct, they move up.
If they run out of time or answers incorrectly, they stay, and the enemy moves up*/

#ifndef PLAYER_H
#define PLAYER_H

#include <SDL2/SDL.h>

class Player {
    public:
        Player();

        void markCorrect();
        void markIncorrect();
        void markTimeOut();

        // Movement
        void moveUp();
        void resetPos();

        // Renderer
        void render(SDL_Renderer* renderer);

        // Getter functions
        int getPosX();
        int getPosY();

        // Setting the starting position and size
        void init(int x, int y, int w, int h, int moveStep);

    private:
        SDL_Rect rect;
        int step;

        bool ranOutOfTime;
        bool isCorrect;
        bool isIncorrect;
};

#endif

