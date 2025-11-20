#include "displayProblem.h"
#include <iostream>
#include <ctime>
#include <cmath>

int SCREEN_WIDTH = 1280;
int SCREEN_HEIGHT = 720;

// Solve system using Gaussian elimination
bool solve(MatrixQuestion& q) {
    int n = q.size;
    double M[3][4];

    for(int i=0;i<n;i++){
        for(int j=0;j<n;j++)
            M[i][j] = q.A[i][j];
        M[i][n] = q.B[i];
    }

    for(int i=0;i<n;i++){
        if(M[i][i] == 0){
            bool swapped = false;
            for(int j=i+1;j<n;j++){
                if(M[j][i] != 0){
                    for(int k=0;k<=n;k++)
                        std::swap(M[i][k], M[j][k]);
                    swapped = true;
                    break;
                }
            }
            if(!swapped) return false;
        }
        for(int j=i+1;j<n;j++){
            double ratio = M[j][i] / M[i][i];
            for(int k=i;k<=n;k++)
                M[j][k] -= ratio * M[i][k];
        }
    }

    for(int i=n-1;i>=0;i--){
        double sum = M[i][n];
        for(int j=i+1;j<n;j++)
            sum -= M[i][j] * q.solution[j];
        q.solution[i] = sum / M[i][i];
    }

    return true;
}

void generateSystem(MatrixQuestion& q, int n) {
    q.size = n;
    int x = std::rand() % 10 + 1;
    int y = std::rand() % 10 + 1;
    int z = std::rand() % 10 + 1;

    for(int i=0;i<n;i++){
        for(int j=0;j<n;j++){
            q.A[i][j] = std::rand()%10 + 1;
        }
    }

    for(int i=0;i<n;i++){
        q.B[i] = q.A[i][0]*x + q.A[i][1]*y + q.A[i][2]*z;
    }

    q.solution[0] = x;
    q.solution[1] = y;
    q.solution[2] = z;
}

void printSystem(const MatrixQuestion& q) {
    int n = q.size;
    for(int i=0;i<n;i++){
        for(int j=0;j<n;j++){
            std::cout << q.A[i][j] << (j==0?"x":j==1?"y":"z");
            if(j<n-1) std::cout << " + ";
        }
        std::cout << " = " << q.B[i] << "\n";
    }
}

bool checkAnswer(MatrixQuestion& q, double userX, double userY, double userZ){
    const double eps = 1e-6;
    return (std::abs(userX - q.solution[0]) < eps &&
            std::abs(userY - q.solution[1]) < eps &&
            std::abs(userZ - q.solution[2]) < eps);
}

void multiply(const int A[4][4], const int B[4][4], int C[4][4], int n) {
    for(int i = 0; i < n; i++) {
        for(int j = 0; j < n; j++) {
            C[i][j] = 0;
            for(int k = 0; k < n; k++) {
                C[i][j] += A[i][k] * B[k][j];
            }
        }
    }
}

// SDL2 Rendering
void renderText(SDL_Renderer* renderer, TTF_Font* font, const std::string& text, int x, int y, SDL_Color color) {
    SDL_Surface* surface = TTF_RenderText_Solid(font, text.c_str(), color);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_Rect dst = {x, y, surface->w, surface->h};
    SDL_RenderCopy(renderer, texture, nullptr, &dst);
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}

void renderMatrixQuestion(SDL_Renderer* renderer, TTF_Font* font, const MatrixQuestion& q, SDL_Color color) {
    int spacing = 50;
    int totalHeight = q.size * spacing;
    int startY = (SCREEN_HEIGHT - totalHeight - 3*spacing) / 2;
    int startX = SCREEN_WIDTH / 2;

    // First, calculate width and height of each line
    for (int i = 0; i < q.size; i++) {
        std::string line;
        for (int j = 0; j < q.size; j++) {
            line += std::to_string(q.A[i][j]) + (j==0?"x":j==1?"y":"z");
            if (j < q.size-1) line += " + ";
        }
        line += " = " + std::to_string(q.B[i]);

        int textW, textH;
        TTF_SizeText(font, line.c_str(), &textW, &textH);

        renderText(renderer, font, line, startX - textW/2, startY + i * spacing, color);
     
    }
}

void renderSolutionLines(SDL_Renderer* renderer, TTF_Font* font, SDL_Color color) {
    int spacing = 60;
    int totalHeight = 3* spacing;
    int startY = (SCREEN_HEIGHT - totalHeight) / 2;
    int startX = SCREEN_WIDTH / 2;

    std::string labels[3] = {"x = ", "y = ", "z = "};

    for (int i = 0; i < 3; i++) {
        int textW, textH;
        TTF_SizeText(font, labels[i].c_str(), &textW, &textH); 

        renderText(renderer, font, labels[i], startX - textW/2, startY + i * spacing, color);
    }
}


int main() {
    std::srand(std::time(0));

    // Matrix multiplication example
    int A[4][4] = { {1,2,3,4}, {5,6,7,8}, {9,10,11,12}, {13,14,15,16} };
    int B[4][4] = { {16,15,14,13}, {12,11,10,9}, {8,7,6,5}, {4,3,2,1} };
    int C[4][4];
    multiply(A, B, C, 4);

    std::cout << "Matrix multiplication result:\n";
    for(int i=0;i<4;i++){
        for(int j=0;j<4;j++){
            std::cout << C[i][j] << "\t";
        }
        std::cout << "\n";
    }

    // Matrix solver
    MatrixQuestion q;
    generateSystem(q, 3);
    std::cout << "\nSolve this system of equations:\n";
    printSystem(q);

    if(!solve(q)){
        std::cout << "No unique solution exists.\n";
        return 0;
    }

    double x, y, z;
    std::cout << "\nEnter your solution:\n";
    std::cout << "x = "; std::cin >> x;
    std::cout << "y = "; std::cin >> y;
    std::cout << "z = "; std::cin >> z;

    if(checkAnswer(q, x, y, z)){
        std::cout << "Correct!";
    } else {
        std::cout << "Incorrect. The correct solution is:\n";
        std::cout << "x = " << q.solution[0] 
                  << ", y = " << q.solution[1] 
                  << ", z = " << q.solution[2] << "\n";
    }

    return 0;
}
