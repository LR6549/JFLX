//
// Perlin noise generation — public interface of the noiseGeneration
// component (src/noiseGeneration.cpp).
//
// The container-based grid variant of the noise generator; a second, header-
// only implementation lives in <mathematics/perlinNoise.hpp> (namespace JFLX).
//

#ifndef JFLX_NOISEGENERATION_HPP
#define JFLX_NOISEGENERATION_HPP

#include <vector>

namespace perlinNoise {

    /// Unit gradient vector of a single noise grid cell.
    struct Gradient {
        double x, y;
    };

    /// Fills the (gridSize+1)² gradient grid with random unit vectors for @p seed.
    void initializeGradients(std::vector<std::vector<Gradient>>& gradients, int gridSize, int seed);

    /// Linear interpolation between @p a and @p b.
    double lerp(double a, double b, double t);

    /// Smoothstep-like weight used to fade the interpolation.
    double fade(double t);

    /// Dot product of a cell gradient with the distance vector.
    double dotGridGradient(const Gradient& gradient, double dx, double dy);

    /// Perlin noise value at (x, y) for a prebuilt gradient grid.
    double perlinNoise(double x, double y, const std::vector<std::vector<Gradient>>& gradients, int gridSize);

    /**
     * @brief Fills @p noiseMap with perlin noise values (0–255).
     * @param noiseMap Noise grid; noiseMap[0].size() = width, size() = height.
     * @param seed     Seed of the gradient grid.
     * @param smoothness Grid size of the gradient grid.
     * @note Takes the map by value, exactly as the original implementation did —
     *       the filled copy is discarded. See the README "Open points".
     */
    void perlinNoise(std::vector<std::vector<int>> noiseMap, int seed, int smoothness);

}

#endif // JFLX_NOISEGENERATION_HPP
