#include <iostream>
#include <vector>
#include <algorithm>
#include <chrono>
#include <cmath>
#include <omp.h>

// Define a structure to represent a circle
struct Circle {
    int x, y, z, radius;
    float transparency; // 0.0 = fully transparent, 1.0 = fully opaque
};

// Function to check if a pixel is within a circle
bool isWithinCircle(int px, int py, const Circle& circle) {
    int dx = px - circle.x;
    int dy = py - circle.y;
    return dx * dx + dy * dy <= circle.radius * circle.radius;
}

// Function to render circles
void renderCircles(const std::vector<Circle>& circles, std::vector<std::vector<float>>& canvas, int width, int height) {
    // Sort circles based on their z-coordinate
    std::vector<Circle> sortedCircles = circles;
    std::sort(sortedCircles.begin(), sortedCircles.end(), [](const Circle& a, const Circle& b) {
        return a.z < b.z;
    });

    // Draw circles on the canvas
    for (const auto& circle : sortedCircles) {
        #pragma omp parallel for
        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                if (isWithinCircle(x, y, circle)) {
                    #pragma omp critical
                    canvas[y][x] = std::max(canvas[y][x], circle.transparency);
                }
            }
        }
    }
}

// Function to print the canvas
void printCanvas(const std::vector<std::vector<float>>& canvas) {
    for (const auto& row : canvas) {
        for (const auto& pixel : row) {
            std::cout << (pixel > 0.0f ? '#' : '.');
        }
        std::cout << '\n';
    }
}

int main() {
    int width = 80, height = 60;  // Smaller canvas for easier visualization
    std::vector<std::vector<float>> canvas(height, std::vector<float>(width, 0.0f));
    std::vector<Circle> circles = {
        {40, 30, 5, 10, 0.5f},
        {20, 15, 3, 8, 0.7f},
        {60, 45, 10, 12, 0.4f}
    };

    auto start = std::chrono::high_resolution_clock::now();
    renderCircles(circles, canvas, width, height);
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;

    std::cout << "Parallel Execution Time: " << elapsed.count() << " seconds\n";

    printCanvas(canvas);

    return 0;
}
