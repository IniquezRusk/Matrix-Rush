#include "enemy.h"
#include <cmath>

Enemy::Enemy() {
    rect = {0, 0, 0, 0};
    step = 0;

    animating = false;
    animCells = 0;
    animDuration = 0.35f;
    animElapsed = 0.0f;
    animStartY = animEndY = 0;
}

void Enemy::init(int x, int y, int w, int h, int moveStep) {
    rect = {x, y, w, h};
    step = moveStep;
    startY = y;
}

void Enemy::moveUp() {
    rect.y -= step;
}

void Enemy::resetPos() {
    // put player back where it started and clear any state/animation
    rect.y = startY;
    rect.x = rect.x; // keep x the same

    // stop any animation
    animating = false;
    animCells = 0;
    animDuration = 0.0f;
    animElapsed = 0.0f;
    animStartY = rect.y;
    animEndY = rect.y;
}

void Enemy::render(SDL_Renderer* renderer) {
    SDL_SetRenderDrawColor(renderer, 255, 20, 60, 255); 
    SDL_RenderFillRect(renderer, &rect);
}

void Enemy::startMoveUp(int cells, float durationSeconds) {
    if (cells <= 0) return;
    if (animating) return;

    animating = true;
    animCells = cells;
    animDuration = (durationSeconds > 0.0f) ? durationSeconds : 0.35f;
    animElapsed = 0.0f;
    animStartY = rect.y;
    animEndY = rect.y - (cells * step);
}

void Enemy::update(float dt) {
    if (!animating) return;

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

int Enemy::getPosX() {
    return rect.x;
}

int Enemy::getPosY() {
    return rect.y;
}

float Enemy::easeOutCubic(float t) {
    if (t <= 0.0f) return 0.0f;
    if (t >= 1.0f) return 1.0f;
    float p = 1.0f - t;
    return 1.0f - p * p * p;
}
