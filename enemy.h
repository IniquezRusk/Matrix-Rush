/* The point of this file is to make an object for the enemy square. It is a simple AI that revolves around the player
(users), actions. If the player gets the answer wrong or runs out of time, the square moves up. If the player answers correctly,
the square will stagnate.*/

#include <SDL2/SDL.h>
#ifndef ENEMY_H
#define ENEMY_H

class Enemy {
    public:
        Enemy();
        void init(int x, int y, int w, int h, int moveSquare);
        void moveUp();
        void startMoveUp(int cells = 1, float durationSections = 0.35f);
        void update(float dt);
        void resetPos();

        void render(SDL_Renderer* renderer);

        int getPosX();
        int getPosY();
    private:
        SDL_Rect rect;
        int step;

        bool animating;
        int animCells;
        float animDuration;
        float animElapsed;
        int animStartY;
        int animEndY;

        int startY;

        static float easeOutCubic(float t);
};

#endif
