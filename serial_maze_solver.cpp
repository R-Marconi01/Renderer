// serial_maze_solver.cpp
#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <stack>
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

void findExit() {
    srand(time(0));
    Position pos = start;
    std::stack<Position> path;
    path.push(pos);

    while (!isExit(pos)) {
        randomMove(pos);
        path.push(pos);
    }

    std::cout << "Exit found at (" << pos.x << ", " << pos.y << ")\n";
    std::cout << "Path to exit:\n";
    while (!path.empty()) {
        Position step = path.top();
        path.pop();
        std::cout << "(" << step.x << ", " << step.y << ")\n";
    }
}

int main() {
    auto start = std::chrono::high_resolution_clock::now();
    findExit();
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;
    std::cout << "Serial execution time: " << elapsed.count() << " seconds\n";
    return 0;
}
