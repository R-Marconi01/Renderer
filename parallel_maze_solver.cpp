// parallel_maze_solver.cpp
#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <omp.h>
#include <chrono>

const int WIDTH = 10;
const int HEIGHT = 10;

enum Direction { UP, DOWN, LEFT, RIGHT };

int maze[HEIGHT][WIDTH] = {
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 0, 0, 0, 1, 0, 1, 0, 0, 1},
    {1, 0, 1, 0, 1, 0, 1, 1, 0, 1},
    {1, 0, 1, 0, 0, 0, 0, 1, 0, 1},
    {1, 0, 1, 1, 1, 1, 0, 1, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 1, 0, 1},
    {1, 1, 1, 1, 1, 1, 0, 1, 0, 1},
    {1, 0, 0, 0, 0, 1, 0, 1, 0, 1},
    {1, 0, 1, 1, 0, 1, 0, 0, 0, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1}
};

struct Position {
    int x;
    int y;
};

Position start = {1, 1};
Position exitPos = {8, 8};

bool isExit(Position pos) {
    return pos.x == exitPos.x && pos.y == exitPos.y;
}

bool isValidMove(Position pos) {
    return pos.x >= 0 && pos.x < WIDTH && pos.y >= 0 && pos.y < HEIGHT && maze[pos.y][pos.x] == 0;
}

void randomMove(Position &pos) {
    Direction dir = static_cast<Direction>(rand() % 4);
    switch (dir) {
        case UP: pos.y--; break;
        case DOWN: pos.y++; break;
        case LEFT: pos.x--; break;
        case RIGHT: pos.x++; break;
    }
    if (!isValidMove(pos)) {
        switch (dir) {
            case UP: pos.y++; break;
            case DOWN: pos.y--; break;
            case LEFT: pos.x++; break;
            case RIGHT: pos.x--; break;
        }
    }
}

void findExitParallel(int numParticles) {
    srand(time(0));
    bool found = false;
    Position finalPos;

    #pragma omp parallel num_threads(numParticles)
    {
        Position pos = start;

        while (!found) {
            randomMove(pos);
            if (isExit(pos)) {
                #pragma omp critical
                {
                    if (!found) {
                        found = true;
                        finalPos = pos;
                    }
                }
            }
        }
    }

    std::cout << "Exit found at (" << finalPos.x << ", " << finalPos.y << ")\n";
}

int main() {
    std::vector<int> particleCounts = {1, 10, 100, 500, 1000}; // Different number of particles for testing
    for (int numParticles : particleCounts) {
        auto start = std::chrono::high_resolution_clock::now();
        findExitParallel(numParticles);
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed = end - start;
        std::cout << "Parallel execution time with " << numParticles << " particles: " << elapsed.count() << " seconds\n";
    }
    return 0;
}
