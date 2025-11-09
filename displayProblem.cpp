#include "displayProblem.h"

void multiply(const int A[4][4], const int B[4][4], int C[4][4], int n) {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            int sum = 0;
            for (int k = 0; k < n; k++) {
                sum += A[i][j] * B[k][j];
            }

            C[i][j] = sum;
        }
    }
}

void randomizeMatrix(MatrixQuestion& question, int n, int range) {

    // Sets values for n, the amount of answers there are

    if (n < 2) {
        n = 2;
    }
    if (n > 4) {
        n = 4;
    }
    if (range < 1) {
        range = 1;
    }

    question.size = n;

    // Fills in A and B with integers in range
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            question.A[i][j] = (std::rand() % (2*range + 1)) - range;
            question.B[i][j] = (std::rand() % (2*range + 1)) - range;
        }
    }

    // Computing C = A * B
    multiply(question.A, question.B, question.correct, n);
}

bool checkAnswer(const MatrixQuestion& question, const int ans[4][4]) {
    const int n = question.size;
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            if (ans[i][j] != question.correct[i][j]) {
                return false;
            } else {
                return true;
            }
        }
    }
}
