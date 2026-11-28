// The TextRenderer public header must compile on its own.
#include <SDL3/TextRenderer.hpp>

// drawText() takes JFLX::Color; SDL_Color values and braced channel lists
// convert implicitly. Compile-time check only — never called.
[[maybe_unused]] static void colorInterop(JFLX::TextRenderer& text, TTF_Font* font, SDL_Color sdlColor) {
    text.drawText("a", 12.f, 0.f, 0.f, font, JFLX::Color(255, 0, 0));
    text.drawText("b", 12.f, 0.f, 0.f, font, sdlColor);
    text.drawText("c", 12.f, 0.f, 0.f, font, {255, 0, 0});
}

int main() { return 0; }
