//
// Umbrella test — <JFLXLIBS.hpp> is the single entry point of the ecosystem
// and must expose every sub-library. umbrella_second.cpp includes the very
// same header in a second translation unit, so linking this target also
// proves that none of the header-only sub-libraries defines a symbol twice.
//

#include <JFLXLIBS.hpp>

#include <cmath>
#include <cstdio>
#include <stdexcept>
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
    ok &= expect(JFLX::config::g_window == nullptr, "g_window starts unset");
    ok &= expect(JFLX::config::g_renderer == nullptr, "g_renderer starts unset");
    // nullptr clears the configuration, so setWindow()/setRenderer() report
    // whether an active resource is stored afterwards.
    ok &= expect(!JFLX::config::setWindow(nullptr), "setWindow(nullptr) clears");
    ok &= expect(!JFLX::config::setRenderer(nullptr), "setRenderer(nullptr) clears");

    // Components build without being handed a window or renderer: they fall
    // back to the (currently unset) global configuration.
    JFLX::TextRenderer text;
    JFLX::TextureHandler textures;
    float width = 0.f, height = 0.f;
    ok &= expect(!text.measureText("x", 12.f, static_cast<TTF_Font*>(nullptr), width, height),
                 "measureText without renderer/font");
    ok &= expect(!textures.renderTexture("jflx_missing", 0.f, 0.f),
                 "renderTexture with no global renderer");

    // ── strings ───────────────────────────────────────────────────────────
    std::string lowered = "JfLx";
    JFLX::strings::toLower(lowered);
    ok &= expect(lowered == "jflx", "JFLX::strings::toLower");

    // ── sorting ───────────────────────────────────────────────────────────
    std::vector<int> values {3, 1, 2};
    JFLX::sort::mergeSort(values);
    ok &= expect(values == std::vector<int>({1, 2, 3}), "JFLX::sort::mergeSort");

    // ── dataStructures ────────────────────────────────────────────────────
    const JFLX::Vector2<double> direction {1.0, 0.0};
    const JFLX::Vector2<double> toOther {1.0, 0.1};
    ok &= expect(direction.dot(toOther) > 0.0, "Vector2::dot");

    // ── mathematics (header-only) ──────────────────────────────────────────
    ok &= expect(JFLX::math::isPrime(7), "JFLX::math::isPrime(7)");
    ok &= expect(!JFLX::math::isPrime(8), "JFLX::math::isPrime(8)");

    // ── mathematics (compiled components: bsa, noise generation) ───────────
    ok &= expect(JFLX::math::calculateµ(10.0, 0.5) == 5.0, "JFLX::math::calculateµ");
    const double expectedSigma = std::sqrt(10.0 * 0.5 * (1.0 - 0.5));
    ok &= expect(std::fabs(JFLX::math::calculateSigma(10.0, 0.5) - expectedSigma) < 1e-9,
                 "JFLX::math::calculateSigma");
    ok &= expect(std::fabs(JFLX::math::calculateSigmaWithMean(0.5, 5.0) - expectedSigma) < 1e-9,
                 "JFLX::math::calculateSigmaWithMean");
    ok &= expect(JFLX::math::calculateSigmaIntervall(10.0, 0.5, 1).size() == 2,
                 "JFLX::math::calculateSigmaIntervall");

    std::vector<std::vector<int>> noiseMap(4, std::vector<int>(4, -1));
    JFLX::math::noise::perlinNoise(noiseMap, 42, 4);   // fills a copy (by-value parameter)
    ok &= expect(noiseMap[0][0] == -1, "perlinNoise leaves the caller's map untouched");

    // ── cryptography ──────────────────────────────────────────────────────
    JFLX::CeasarCypher cipher;
    ok &= expect(cipher.encrypt(1, "a") == "b", "CeasarCypher::encrypt");

    // ── positioning ───────────────────────────────────────────────────────
    ok &= expect(JFLX::positioning::isInFOV(direction, toOther, 0.45), "isInFOV inside cone");
    ok &= expect(!JFLX::positioning::isInFOV(direction, JFLX::Vector2<double>{-1.0, 0.0}, 0.45),
                 "isInFOV behind");

    // ── utilities ─────────────────────────────────────────────────────────
    JFLX::utilities::logDebugInfo("umbrella test");   // silent: the -d flag is off by default

    // ── color ─────────────────────────────────────────────────────────────
    const JFLX::Color fromRgb(255, 136, 0);
    ok &= expect(fromRgb.r == 255 && fromRgb.g == 136 && fromRgb.b == 0 && fromRgb.a == 255,
                 "Color from RGB");
    const JFLX::Color fromHex("#ff8800");
    ok &= expect(fromHex == fromRgb, "Color from hex");
    ok &= expect(fromHex.toHex() == "#ff8800", "Color::toHex");
    const JFLX::Color fromShortHex("#f80");
    ok &= expect(fromShortHex == fromRgb, "Color from short hex");
    ok &= expect(JFLX::color::hexToRgb("00FF8880") == JFLX::Color(0, 255, 136, 128),
                 "JFLX::color::hexToRgb with alpha");
    ok &= expect(JFLX::color::rgbToHex(0, 255, 136, 128) == "#00ff8880",
                 "JFLX::color::rgbToHex with alpha");
    bool threw = false;
    try {
        (void) JFLX::color::hexToRgb("#nothex");
    } catch (const std::invalid_argument&) {
        threw = true;
    }
    ok &= expect(threw, "hexToRgb rejects malformed input");

    // ── json (optional dependency) ─────────────────────────────────────────
#if defined(JFLX_JSON_HAS_NLOHMANN)
    nlohmann::json jflxDoc;
    ok &= expect(JFLX::json::parseJson("{\"player\": {\"stats\": {\"hp\": 7}}}", jflxDoc),
                 "JFLX::json::parseJson");
    nlohmann::json jflxValue;
    ok &= expect(JFLX::json::getJsonByPath(jflxDoc, "player.stats.hp", jflxValue) && jflxValue == 7,
                 "JFLX::json::getJsonByPath");
    ok &= expect(JFLX::json::setJsonByPath(jflxDoc, "player.stats.mp", 3) &&
                 JFLX::json::hasJsonPath(jflxDoc, "player.stats.mp"),
                 "JFLX::json::setJsonByPath / JFLX::json::hasJsonPath");
#endif

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
