#include <iostream>
#include <vector>
#include <algorithm>
#include <chrono>
#include <map>
#include <string>

struct Circle {
    int x, y, z, radius;
    char color; // Character representing the color
};

bool isWithinCircle(int px, int py, const Circle& circle) {
    int dx = px - circle.x;
    int dy = py - circle.y;
    return dx * dx + dy * dy <= circle.radius * circle.radius;
}

void renderCircles(int numthreads, const std::vector<Circle>& circles, std::vector<std::vector<std::string>>& canvas, int width, int height) {
    std::vector<Circle> sortedCircles = circles;
    std::sort(sortedCircles.begin(), sortedCircles.end(), [](const Circle& a, const Circle& b) {
        return a.z > b.z; // Sort from highest to lowest z to manage precedence
    });

    for (const auto& circle : sortedCircles) {
        #pragma omp parallel for num_threads(numthreads)
        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                if (isWithinCircle(x, y, circle)) {
                    #pragma omp critical
                    canvas[y][x] += circle.color; // Add the color to the string at the pixel
                }
            }
        }
    }
}

void printCanvas(const std::vector<std::vector<std::string>>& canvas) {
    std::map<std::string, char> colorCodes = { // Map to translate combinations into symbols
        {"R", 'R'}, {"G", 'G'}, {"B", 'B'},
        {"RG", '1'}, {"RB", '2'}, {"GB", '3'},
        {"GR", '4'}, {"BR", '5'}, {"BG", '6'},
        {"RGB", '7'}, {"RBG", '8'}, {"GRB", '9'},
        {"GBR", 'A'}, {"BRG", 'B'}, {"BGR", 'C'}
    };

    for (const auto& row : canvas) {
        for (const auto& pixel : row) {
            std::cout << (colorCodes.count(pixel) ? colorCodes[pixel] : '.');
        }
        std::cout << '\n';
    }
}

int main() {
     int width = 100, height = 60;
    std::vector<std::vector<std::string>> canvas(height, std::vector<std::string>(width, ""));
    std::vector<Circle> circles;

    // Define the z-orders and positions for the desired overlapping patterns
    std::vector<std::vector<int>> zOrders = {
        {0, 1, 2}, {0, 2, 1}, {1, 0, 2}, // First row
        {1, 2, 0}, {2, 0, 1}, {2, 1, 0}  // Second row
    };

    int xOffset = 20;
    int yOffset = 15;
    int zBase = 0;
    for (int i = 0; i < 2; ++i) { // Two rows
        for (int j = 0; j < 3; ++j) { // Three triplets per row
            int idx = i * 3 + j;
            circles.push_back({xOffset + j * 20, yOffset + i * 20, zBase + zOrders[idx][0], 5, 'R'});
            circles.push_back({xOffset + j * 20 + 2, yOffset + i * 20 + 1, zBase + zOrders[idx][1], 5, 'G'});
            circles.push_back({xOffset + j * 20 + 4, yOffset + i * 20 + 2, zBase + zOrders[idx][2], 5, 'B'});
        }
        zBase += 10;  // Ensure unique global Z values for each new row
    }

    std::vector<int> threadCounts = {1, 2, 3, 4}; // Different number of threads for testing
    for (int numthreads : threadCounts) {
        auto start = std::chrono::high_resolution_clock::now();
        renderCircles(numthreads, circles, canvas, width, height);
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed = end - start;

        std::cout << "Parallel execution time with " << numthreads << " threads: " << elapsed.count() << " seconds\n";
    }
    return 0;
    

     std::cout << "Legend: R=Red, G=Green, B=Blue, 1=RG,\n 2=RB, 3=GB, 4=GR, 5=BR, 6=BG, "
              << "7=RGB,\n 8=RBG, 9=GRB, A=GBR, B=BRG, C=BGR\n";

    printCanvas(canvas);

    return 0;
}
