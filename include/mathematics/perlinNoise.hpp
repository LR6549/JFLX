#ifndef JFLX_PERLINNOISE
#define JFLX_PERLINNOISE

#include <iostream>
#include <cmath>
#include <cstdlib>

#define PI 3.14159265358979323846

namespace JFLX {
    struct Gradient {
        double x, y;
    };

    // Initialize gradients in a 1D array representing 2D
    inline void initializeGradients(Gradient* gradients, int gridSize, int seed) {
        srand(seed);
        for (int i = 0; i <= gridSize; ++i) {
            for (int j = 0; j <= gridSize; ++j) {
                double angle = ((double)rand() / RAND_MAX) * 2 * PI;
                gradients[i * (gridSize + 1) + j] = {cos(angle), sin(angle)};
            }
        }
    }

    inline double lerp(double a, double b, double t) {
        return a + t * (b - a);
    }

    inline double fade(double t) {
        return t * t * t * (t * (t * 6 - 15) + 10);
    }

    inline double dotGridGradient(const Gradient &gradient, double dx, double dy) {
        return gradient.x * dx + gradient.y * dy;
    }

    inline double perlinNoise(double x, double y, const Gradient* gradients, int gridSize) {
        int x0 = (int)floor(x) % gridSize;
        int y0 = (int)floor(y) % gridSize;
        int x1 = (x0 + 1) % gridSize;
        int y1 = (y0 + 1) % gridSize;

        double dx = x - floor(x);
        double dy = y - floor(y);

        double dot00 = dotGridGradient(gradients[x0 * (gridSize + 1) + y0], dx, dy);
        double dot10 = dotGridGradient(gradients[x1 * (gridSize + 1) + y0], dx - 1, dy);
        double dot01 = dotGridGradient(gradients[x0 * (gridSize + 1) + y1], dx, dy - 1);
        double dot11 = dotGridGradient(gradients[x1 * (gridSize + 1) + y1], dx - 1, dy - 1);

        double u = fade(dx);
        double v = fade(dy);

        double lerpX1 = lerp(dot00, dot10, u);
        double lerpX2 = lerp(dot01, dot11, u);
        return lerp(lerpX1, lerpX2, v);
    }

    inline void perlinNoise(int* noiseMap, int width, int height, int seed, int smoothness) {
        int gridSize = smoothness;

        Gradient* gradients = new Gradient[(gridSize + 1) * (gridSize + 1)];
        initializeGradients(gradients, gridSize, seed);

        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                double nx = (double)x / width * gridSize;
                double ny = (double)y / height * gridSize;

                double noiseValue = perlinNoise(nx, ny, gradients, gridSize);

                noiseMap[y * width + x] = static_cast<int>(255 * ((noiseValue + 1) / 2.0));
            }
        }

        delete[] gradients;
    }
}

#endif
