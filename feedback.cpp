#include "feedback.h"
const int SCREEN_WIDTH = 1280;

Feedback::Feedback() {
    active = false;
    correct = false;
    timer = 0.0f;
    blinkCount = 0;
    showSolutionTime = 5.0f;
    showingSolution = false;
}

void Feedback::showCorrect() {
    active = true;
    correct = true;
    timer = 1.0;
    blinkCount = 0;
    showingSolution = false;
}

void Feedback::showIncorrect() {
    active = true;
    correct = false;
    timer = 0.5f;
    blinkCount = 0;
    showingSolution = false;
}

void Feedback::update(float dt) {
    if (!active) return;

    timer -= dt;

    if (correct) {
        if (timer <= 0.0f) active = false;
    } else {
        if (!showingSolution) {
            if (timer <= 0.0f) {
                blinkCount++;
                if (blinkCount < 4) {
                    timer = 0.5f; // blink interval
                } else {
                    showingSolution = true; // start showing solution
                    timer = showSolutionTime;
                }
            }
        } else {
            if (timer <= 0.0f) {
                active = false;
            }
        }
    }
}

void Feedback::render(SDL_Renderer* renderer, TTF_Font* font, const MatrixQuestion& q) {
    if (!active) return;

    SDL_Color green = {0, 255, 0, 255};
    SDL_Color red = {255, 0, 0, 255};

    int textW, textH;
    int centerX = SCREEN_WIDTH / 2;

    if (correct) {
        std::string msg = "[CORRECT]";
        TTF_SizeText(font, msg.c_str(), &textW, &textH);
        renderText(renderer, font, msg, centerX - textW / 2, 50, green);
    } else if (blinkCount % 2 == 0) {
        std::string msg = "[INCORRECT]";
        TTF_SizeText(font, msg.c_str(), &textW, &textH);
        renderText(renderer, font, msg, centerX - textW / 2, 50, red);

        // Solution text
        std::string solution = "x=" + std::to_string(q.solution[0]) +
                               ", y=" + std::to_string(q.solution[1]) +
                               ", z=" + std::to_string(q.solution[2]);
        TTF_SizeText(font, solution.c_str(), &textW, &textH);
        renderText(renderer, font, solution, centerX - textW / 2, 100, red);
    }
}
