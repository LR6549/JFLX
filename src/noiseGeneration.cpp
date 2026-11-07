#include <mathematics/noiseGeneration.hpp>

#include <iostream>
#include <vector>
#include <cmath>
#include <cstdlib>

#define PI 3.14159265358979323846

using namespace std;

namespace perlinNoise {

    // Function to initialize gradients with a random seed
    void initializeGradients(vector<vector<Gradient>> &gradients, int gridSize, int seed) {
        srand(seed);
        for (int i = 0; i <= gridSize; ++i) {
            for (int j = 0; j <= gridSize; ++j) {
                double angle = ((double)rand() / RAND_MAX) * 2 * PI;
                gradients[i][j] = {cos(angle), sin(angle)};
            }
        }
    }

    // Linear interpolation function
    double lerp(double a, double b, double t) {
        return a + t * (b - a);
    }

    // Fade function for smooth transitions
    double fade(double t) {
        return t * t * t * (t * (t * 6 - 15) + 10);
    }

    // Dot product of the gradient vector and the distance vector
    double dotGridGradient(const Gradient &gradient, double dx, double dy) {
        return gradient.x * dx + gradient.y * dy;
    }

    // Perlin noise function
    double perlinNoise(double x, double y, const vector<vector<Gradient>> &gradients, int gridSize) {
        int x0 = (int)floor(x) % gridSize;
        int y0 = (int)floor(y) % gridSize;
        int x1 = (x0 + 1) % gridSize;
        int y1 = (y0 + 1) % gridSize;

        double dx = x - floor(x);
        double dy = y - floor(y);

        // Get dot products from the grid corners
        double dot00 = dotGridGradient(gradients[x0][y0], dx, dy);
        double dot10 = dotGridGradient(gradients[x1][y0], dx - 1, dy);
        double dot01 = dotGridGradient(gradients[x0][y1], dx, dy - 1);
        double dot11 = dotGridGradient(gradients[x1][y1], dx - 1, dy - 1);

        // Interpolate values
        double u = fade(dx);
        double v = fade(dy);

        double lerpX1 = lerp(dot00, dot10, u);
        double lerpX2 = lerp(dot01, dot11, u);
        return lerp(lerpX1, lerpX2, v);
    }

    void perlinNoise(vector<vector<int>> noiseMap, int seed, int smoothness) {
        int width = noiseMap[0].size();
        int height = noiseMap.size();

        int gridSize = smoothness;

        // Initialize gradients
        vector<vector<Gradient>> gradients(gridSize + 1, vector<Gradient>(gridSize + 1));
        initializeGradients(gradients, gridSize, seed);

        // Generate Perlin noise
        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                double nx = (double)x / width * gridSize;
                double ny = (double)y / height * gridSize;

                double noiseValue = perlinNoise(nx, ny, gradients, gridSize);

                int normalizedValue = static_cast<int>(255*((noiseValue + 1) / 2.0));
                noiseMap[y][x] = normalizedValue;
            }
        }

    }
}