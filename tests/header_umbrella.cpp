//
// Umbrella test — <JFLXLIBS.hpp> is the single entry point of the ecosystem
// and must expose every sub-library. umbrella_second.cpp includes the very
// same header in a second translation unit, so linking this target also
// proves that none of the header-only sub-libraries defines a symbol twice.
//

#include <JFLXLIBS.hpp>

#include <cmath>
#include <cstdio>
#include <string>
#include <vector>

/// Defined in umbrella_second.cpp (second TU including <JFLXLIBS.hpp>).
int umbrellaSecondTranslationUnit();

namespace {

    bool expect(bool value, const char* what) {
        if (!value) {
            std::fprintf(stderr, "FAIL: %s\n", what);
        }
        return value;
    }

} // namespace

int main() {
    bool ok = true;

    // ── SDL3: global window/renderer configuration ────────────────────────
    ok &= expect(JFLX::SDL3::CONFIG::g_window == nullptr, "g_window starts unset");
    ok &= expect(JFLX::SDL3::CONFIG::g_renderer == nullptr, "g_renderer starts unset");
    // nullptr clears the configuration, so setWindow()/setRenderer() report
    // whether an active resource is stored afterwards.
    ok &= expect(!JFLX::SDL3::CONFIG::setWindow(nullptr), "setWindow(nullptr) clears");
    ok &= expect(!JFLX::SDL3::CONFIG::setRenderer(nullptr), "setRenderer(nullptr) clears");

    // Components build without being handed a window or renderer: they fall
    // back to the (currently unset) global configuration.
    JFLX::SDL3::TextRenderer text;
    JFLX::SDL3::TextureHandler textures;
    float width = 0.f, height = 0.f;
    ok &= expect(!text.measureText("x", 12.f, static_cast<TTF_Font*>(nullptr), width, height),
                 "measureText without renderer/font");
    ok &= expect(!textures.renderTexture("jflx_missing", 0.f, 0.f),
                 "renderTexture with no global renderer");

    // ── strings ───────────────────────────────────────────────────────────
    std::string lowered = "JfLx";
    JFLX::toLower(lowered);
    ok &= expect(lowered == "jflx", "JFLX::toLower");

    // ── sorting ───────────────────────────────────────────────────────────
    std::vector<int> values {3, 1, 2};
    JFLX::MergeSort::sort(values);
    ok &= expect(values == std::vector<int>({1, 2, 3}), "JFLX::MergeSort::sort");

    // ── dataStructures ────────────────────────────────────────────────────
    const JFLX::vects::Vector2<double> direction {1.0, 0.0};
    const JFLX::vects::Vector2<double> toOther {1.0, 0.1};
    ok &= expect(direction.dot(toOther) > 0.0, "Vector2::dot");

    // ── mathematics (header-only) ──────────────────────────────────────────
    ok &= expect(PrimeNumbers::isPrime(7), "PrimeNumbers::isPrime(7)");
    ok &= expect(!PrimeNumbers::isPrime(8), "PrimeNumbers::isPrime(8)");

    // ── mathematics (compiled components: bsa, noise generation) ───────────
    ok &= expect(bsa::calculateµ(10.0, 0.5) == 5.0, "bsa::calculateµ");
    const double expectedSigma = std::sqrt(10.0 * 0.5 * (1.0 - 0.5));
    ok &= expect(std::fabs(bsa::calculateSigma(10.0, 0.5) - expectedSigma) < 1e-9,
                 "bsa::calculateSigma");
    ok &= expect(std::fabs(bsa::calculateSigmaWithMean(0.5, 5.0) - expectedSigma) < 1e-9,
                 "bsa::calculateSigmaWithMean");
    ok &= expect(bsa::calculateSigmaIntervall(10.0, 0.5, 1).size() == 2,
                 "bsa::calculateSigmaIntervall");

    std::vector<std::vector<int>> noiseMap(4, std::vector<int>(4, -1));
    perlinNoise::perlinNoise(noiseMap, 42, 4);   // fills a copy (by-value parameter)
    ok &= expect(noiseMap[0][0] == -1, "perlinNoise leaves the caller's map untouched");

    // ── cryptography ──────────────────────────────────────────────────────
    ceasarCypher::CeasarCypher cipher;
    ok &= expect(cipher.encrypt(1, "a") == "b", "CeasarCypher::encrypt");

    // ── positioning ───────────────────────────────────────────────────────
    ok &= expect(isInFOV(direction, toOther, 0.45), "isInFOV inside cone");
    ok &= expect(!isInFOV(direction, JFLX::vects::Vector2<double>{-1.0, 0.0}, 0.45),
                 "isInFOV behind");

    // ── utilities ─────────────────────────────────────────────────────────
    logDebugInfo("umbrella test");   // silent: the -d flag is off by default

    // ── network (optional dependencies) ───────────────────────────────────
#if defined(JFLX_NETWORK_HAS_CURL) && defined(JFLX_NETWORK_HAS_JSON)
    ok &= expect(JFLX_NETWORK_HAS_CURL == 1 && JFLX_NETWORK_HAS_JSON == 1,
                 "network wrappers reachable");
#endif

    // ── second translation unit including the same umbrella header ────────
    ok &= expect(umbrellaSecondTranslationUnit() == 1, "second TU");

    if (!ok) {
        std::fputs("JFLXLIBS umbrella test FAILED\n", stderr);
        return 1;
    }

    std::puts("JFLXLIBS umbrella test OK");
    return 0;
}
