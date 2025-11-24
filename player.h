#ifndef PLAYER_H
#define PLAYER_H

#include <SDL2/SDL.h>

class Player {
public:
    Player();

    void init(int x, int y, int w, int h, int moveStep);

    void markCorrect();
    void markIncorrect();
    void markTimeOut();

    // instant move (legacy)
    void moveUp();

    // start smooth move up by N cells over durationSeconds (non-blocking)
    void startMoveUp(int cells = 1, float durationSeconds = 0.35f);

    // advance animation by dt seconds
    void update(float dt);

    void resetPos();

    int getPosX();
    int getPosY();

    void render(SDL_Renderer* renderer);

private:
    SDL_Rect rect;
    int step;

    bool ranOutOfTime;
    bool isCorrect;
    bool isIncorrect;

    // animation state
    bool animating;
    int animCells;
    float animDuration;
    float animElapsed;
    int animStartY;
    int animEndY;

    // add this
    int startY;

    static float easeOutCubic(float t);

};

#endif

