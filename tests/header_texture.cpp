// The TextureHandler public header must compile on its own.
#include <SDL3/TextureHandler.hpp>

// renderTexture() takes JFLX::Color; SDL_Color values and braced channel lists
// convert implicitly. Compile-time check only — never called.
[[maybe_unused]] static void colorInterop(JFLX::TextureHandler& textures, SDL_Color sdlColor) {
    textures.renderTexture(0, 0.f, 0.f, JFLX::renderMode::JFLX_RENDER_CENTERED,
                           1.f, 1.f, 0.0, SDL_FLIP_NONE, JFLX::Color(255, 0, 0));
    textures.renderTexture("player", 0.f, 0.f, JFLX::renderMode::JFLX_RENDER_CENTERED,
                           1.f, 1.f, 0.0, SDL_FLIP_NONE, sdlColor);
    textures.renderTexture(0, 0.f, 0.f, JFLX::renderMode::JFLX_RENDER_CENTERED,
                           1.f, 1.f, 0.0, SDL_FLIP_NONE, {255, 0, 0});
}

int main() { return 0; }
