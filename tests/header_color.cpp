// The color public header must compile and link on its own, and the
// hex <-> RGB(A) conversions must round-trip.
#include <color/color.hpp>

#include <cstdio>
#include <stdexcept>

int main() {
    bool ok = true;

    // hex -> Color -> hex round-trip (long, short and alpha forms)
    ok &= (JFLX::Color("#c0ffee").toHex() == "#c0ffee");
    ok &= (JFLX::color::hexToRgb("C0FFEE").toHex() == "#c0ffee");
    ok &= (JFLX::Color("#1234").toHex(true) == "#11223344");
    ok &= (JFLX::color::hexToRgb("#abc").toHex() == "#aabbcc");
    ok &= (JFLX::Color(1, 2, 3).toHex(true) == "#010203ff");

    // rgb -> hex -> rgb round-trip
    const JFLX::Color source(12, 34, 56, 78);
    ok &= (JFLX::color::hexToRgb(JFLX::color::rgbToHex(source, true)) == source);
    ok &= (JFLX::color::rgbToHex(12, 34, 56) == "#0c2238");

    // blending
    const JFLX::Color black(0, 0, 0, 0);
    const JFLX::Color white(255, 255, 255, 255);
    ok &= (JFLX::color::blend(black, white, 0.0) == black);
    ok &= (JFLX::color::blend(black, white, 1.0) == white);
    ok &= (JFLX::color::blend(black, white, 0.5) == JFLX::Color(128, 128, 128, 128));
    ok &= (JFLX::color::blendPercent(black, white, 50.0) == JFLX::color::blend(black, white, 0.5));
    ok &= (JFLX::color::blendPercent(black, white, 0.0) == black);
    ok &= (JFLX::color::blendPercent(black, white, 100.0) == white);
    // out-of-range blend degrees are clamped
    ok &= (JFLX::color::blend(black, white, -3.0) == black);
    ok &= (JFLX::color::blend(black, white, 42.0) == white);
    ok &= (JFLX::color::blendPercent(black, white, -3.0) == black);
    ok &= (JFLX::color::blendPercent(black, white, 4200.0) == white);
    // a blend result is a plain Color value
    ok &= (JFLX::color::blend(JFLX::Color("#000000"), JFLX::Color("#ffffff"), 0.5).toHex() == "#808080");

    // malformed input throws
    for (const char* bad : {"#12", "#12345", "#gggggg", ""}) {
        try {
            (void) JFLX::color::hexToRgb(bad);
            ok = false;
        } catch (const std::invalid_argument&) {
        }
    }

    if (!ok) {
        std::fputs("JFLXLIBS color header test FAILED\n", stderr);
        return 1;
    }

    std::puts("JFLXLIBS color header test OK");
    return 0;
}
