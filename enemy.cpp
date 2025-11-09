#include "enemy.h"

Enemy::Enemy() {
    rect = {0, 0, 0, 0};
    step = 0;
}

void Enemy::init(int x, int y, int w, int h, int moveStep) {
    rect = {x, y, w, h};
    step = moveStep;
}

void Enemy::moveUp() {
    rect.y -= step;
}

void Enemy::resetPos() {
    /* Will edit later if necessary */
}

void Enemy::render(SDL_Renderer* renderer) {
    SDL_SetRenderDrawColor(renderer, 255, 20, 60, 255); 
    SDL_RenderFillRect(renderer, &rect);
}

int Enemy::getPosX() {
    return rect.x;
}

int Enemy::getPosY() {
    return rect.y;
}
