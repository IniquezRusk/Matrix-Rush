#include "player.h"

Player::Player() {
    ranOutOfTime = false;
    isCorrect = false;
    isIncorrect = false;
    step = 0;
    rect = {0, 0, 0, 0};
}

void Player::init(int x, int y, int w, int h, int moveStep) {
    rect = {x, y, w, h};
    step = moveStep;
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
void Player::resetPos() {
    /* Something extra, might be used later but here just because why not */
}

int Player::getPosX() {
    return rect.x;
}

int Player::getPosY() {
    return rect.y;
}

void Player::render(SDL_Renderer* renderer) {
    SDL_SetRenderDrawColor(renderer, 0, 255, 120, 255);
    SDL_RenderFillRect(renderer, &rect);
}
