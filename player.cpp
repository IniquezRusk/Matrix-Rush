#include "player.h"
#include <cmath>

// constructor
Player::Player()
: step(0),
  ranOutOfTime(false),
  isCorrect(false),
  isIncorrect(false),
  startY(0),
  animating(false),
  animCells(0),
  animDuration(0.0f),
  animElapsed(0.0f),
  animStartY(0),
  animEndY(0)
{
    rect = {0,0,0,0};
}

void Player::init(int x, int y, int w, int h, int moveStep) {
    rect = {x, y, w, h};
    step = moveStep;
    startY = y;
}

void Player::markCorrect() {
    ranOutOfTime = false;
    isIncorrect = false;
    isCorrect = true;
}

void Player::markIncorrect() {
    ranOutOfTime = false;
    isIncorrect = true;
    isCorrect = false;
}

void Player::markTimeOut() {
    ranOutOfTime = true;
    isIncorrect = false;
    isCorrect  = false;
}

void Player::moveUp() {
    rect.y -= step;
}

void Player::startMoveUp(int cells, float durationSeconds) {
    if (cells <= 0) return;
    if (animating) return; // don't restart mid-animation

    animating = true;
    animCells = cells;
    animDuration = (durationSeconds > 0.0f) ? durationSeconds : 0.35f;
    animElapsed = 0.0f;
    animStartY = rect.y;
    animEndY = rect.y - (cells * step);
}

void Player::update(float dt) {
    if (!animating) return;

    // accumulate elapsed time (bugfix: was resetting to dt)
    animElapsed += dt;
    float t = animElapsed / animDuration;
    if (t >= 1.0f) {
        rect.y = animEndY;
        animating = false;
        return;
    }

    float e = easeOutCubic(t);
    float curY = animStartY + (animEndY - animStartY) * e;
    rect.y = static_cast<int>(std::round(curY));
}

void Player::resetPos() {
    // put player back where it started and clear any state/animation
    rect.y = startY;
    rect.x = rect.x; // keep x the same
    ranOutOfTime = false;
    isCorrect = false;
    isIncorrect = false;

    // stop any animation
    animating = false;
    animCells = 0;
    animDuration = 0.0f;
    animElapsed = 0.0f;
    animStartY = rect.y;
    animEndY = rect.y;
}

int Player::getPosX() { return rect.x; }
int Player::getPosY() { return rect.y; }

void Player::render(SDL_Renderer* renderer) {
    SDL_SetRenderDrawColor(renderer, 0, 255, 120, 255);
    SDL_RenderFillRect(renderer, &rect);
}

float Player::easeOutCubic(float t) {
    if (t <= 0.0f) return 0.0f;
    if (t >= 1.0f) return 1.0f;
    float p = 1.0f - t;
    return 1.0f - p*p*p;
}
