//
// Created by lenny on 22.01.2026.
//

#include <SDL3/TextRenderer.hpp>

#include <utilities/logging.hpp>

#include <cstddef>
#include <functional>
#include <string>
#include <utility>

JFLX::SDL3::TextRenderer::TextRenderer() = default;

JFLX::SDL3::TextRenderer::TextRenderer(SDL_Renderer* r) : defaultRenderer(r) {}

JFLX::SDL3::TextRenderer::~TextRenderer() {
    cleanUp();
}

void JFLX::SDL3::TextRenderer::setDefaultRenderer(SDL_Renderer *renderer) {
    if (renderer != defaultRenderer) {
        // Cached textures belong to the renderer they were created on.
        clearTextCache();
    }
    defaultRenderer = renderer;
}

TTF_Font* JFLX::SDL3::TextRenderer::getFont(int index) const {
    if (index < 0 || index >= static_cast<int>(fonts.size())) {
        return nullptr;
    } return fonts[index];
}

TTF_Font* JFLX::SDL3::TextRenderer::getFont(const std::string& fontName) const {
    if (!fontIndex.contains(fontName)) {
        return nullptr;
    } return fonts[fontIndex.at(fontName)];
}

void JFLX::SDL3::TextRenderer::loadFont(const std::string& pathToFont, const std::string& fontName) {
    TTF_Font* tempFont = TTF_OpenFont(pathToFont.c_str(), 32);
    if (!tempFont) {
        JFLX::log("Failed to load font ", (pathToFont + ", " + SDL_GetError()), JFLX::LOGTYPE::JFLX_ERROR);
        exit(1);
    }
    int index = static_cast<int>(fonts.size());
    fonts.push_back(tempFont);

    if (!fontName.empty()) {
        fontIndex[fontName] = index;
    }
}

bool JFLX::SDL3::TextRenderer::measureText(const std::string& text, float fontSize, TTF_Font* font, float& width, float& height) const {
    if (!font) {
        JFLX::log("Font is nullptr: ", "", JFLX::LOGTYPE::JFLX_ERROR);
        return false;
    }

    TTF_SetFontSize(font, static_cast<int>(fontSize));

    int w = 0;
    int h = 0;
    if (!TTF_GetStringSize(font, text.c_str(), text.size(), &w, &h)) {
        JFLX::log("Failed to measure text: ", SDL_GetError(), JFLX::LOGTYPE::JFLX_ERROR);
        return false;
    }

    width = static_cast<float>(w);
    height = static_cast<float>(h);
    return true;
}

bool JFLX::SDL3::TextRenderer::measureText(const std::string& text, float fontSize, int fontIndex_, float& width, float& height) const {
    return measureText(text, fontSize, getFont(fontIndex_), width, height);
}

bool JFLX::SDL3::TextRenderer::measureText(const std::string& text, float fontSize, const std::string& fontName, float& width, float& height) const {
    return measureText(text, fontSize, getFont(fontName), width, height);
}

bool JFLX::SDL3::TextRenderer::unloadFont(int index) {
    if (index < 0 || index >= static_cast<int>(fonts.size())) {
        return false;
    }

    TTF_Font* font = fonts[index];

    // Drop cached textures that were rasterised with this font.
    for (auto it = textCache.begin(); it != textCache.end(); ) {
        if (it->first.font == font) {
            if (it->second.text) SDL_DestroyTexture(it->second.text);
            if (it->second.outline) SDL_DestroyTexture(it->second.outline);
            it = textCache.erase(it);
        } else {
            ++it;
        }
    }

    TTF_CloseFont(font);
    fonts.erase(fonts.begin() + index);

    // Re-map name → index entries: drop names for this font, shift later ones.
    for (auto it = fontIndex.begin(); it != fontIndex.end(); ) {
        if (it->second == index) {
            it = fontIndex.erase(it);
        } else {
            if (it->second > index) --it->second;
            ++it;
        }
    }
    return true;
}

bool JFLX::SDL3::TextRenderer::unloadFont(const std::string& fontName) {
    const auto it = fontIndex.find(fontName);
    if (it == fontIndex.end()) {
        return false;
    }
    return unloadFont(it->second);
}

size_t JFLX::SDL3::TextRenderer::TextCacheKeyHash::operator()(const TextCacheKey& key) const noexcept {
    const auto voidHash = std::hash<const void*>{};

    size_t seed = voidHash(key.renderer);
    const auto mix = [&seed](size_t value) {
        seed ^= value + 0x9e3779b97f4a7c15ULL + (seed << 6) + (seed >> 2);
    };

    mix(voidHash(key.font));
    mix(std::hash<int>{}(key.size));
    mix(std::hash<std::string>{}(key.text));
    mix((static_cast<size_t>(key.color.r) << 24) |
        (static_cast<size_t>(key.color.g) << 16) |
        (static_cast<size_t>(key.color.b) << 8)  |
         static_cast<size_t>(key.color.a));
    mix(static_cast<size_t>(key.outline));
    return seed;
}

void JFLX::SDL3::TextRenderer::drawText(const std::string& text, float fontSize, float x, float y, TTF_Font* font, const SDL_Color color, int orientation, bool outline, SDL_Renderer* rRenderer) const {
    if (!font) {
        JFLX::log("Font is nullptr: ", "", JFLX::LOGTYPE::JFLX_ERROR);
        return;
    }

    SDL_Renderer* renderer = rRenderer ? rRenderer
                                       : (defaultRenderer ? defaultRenderer : CONFIG::g_renderer);

    if (!renderer) {
        JFLX::log("Renderer is nullptr: ", "", JFLX::LOGTYPE::JFLX_ERROR);
        return;
    }

    const int pxSize = static_cast<int>(fontSize);
    const TextCacheKey key { renderer, font, pxSize, color, outline, text };

    auto it = textCache.find(key);
    if (it == textCache.end()) {
        // Cache miss — rasterise once; every later identical drawText() reuses
        // this texture instead of re-rendering and re-uploading it.
        TTF_SetFontSize(font, pxSize);

        SDL_Surface* surface = TTF_RenderText_Blended(font, text.c_str(), text.size(), color);
        if (!surface) {
            JFLX::log("Failed to render text: ", SDL_GetError(), JFLX::LOGTYPE::JFLX_ERROR);
            return;
        }

        // Create texture BEFORE destroying surface
        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_DestroySurface(surface);

        if (!texture) {
            JFLX::log("Failed to create text texture: ", SDL_GetError(), JFLX::LOGTYPE::JFLX_ERROR);
            return;
        }

        SDL_Texture* outlineTexture = nullptr;
        if (outline) {
            SDL_Surface* outlineSurface = TTF_RenderText_Blended(font, text.c_str(), text.size(), {0, 0, 0, 255});
            if (outlineSurface) {
                outlineTexture = SDL_CreateTextureFromSurface(renderer, outlineSurface);
                SDL_DestroySurface(outlineSurface);
                // If outline creation fails the main text is still drawn,
                // matching the previous behavior — just without an outline.
            }
        }

        it = textCache.emplace(std::move(key), CachedText { texture, outlineTexture }).first;
    }

    const CachedText& cached = it->second;

    float textW, textH;
    SDL_GetTextureSize(cached.text, &textW, &textH);

    // Position based on orientation
    SDL_FRect dstRect;
    dstRect.w = textW;
    dstRect.h = textH;

    if (orientation == 0)       dstRect.x = x - (textW / 2.0f);
    else if (orientation == -1) dstRect.x = x - textW;
    else                        dstRect.x = x;

    dstRect.y = y;

    // Draw outline/shadow first (underneath the main text)
    if (cached.outline) {
        SDL_FRect outlineRect = dstRect;

        // Offset relative to fontSize
        const float offset = fontSize / 6.5f;

        // All 8 directions for a solid outline effect
        const SDL_FPoint offsets[] = {
            {-offset, -offset}, {0, -offset}, {offset, -offset},
            {-offset,  0},                    {offset,  0},
            {-offset,  offset}, {0,  offset}, {offset,  offset}
        };

        for (const auto& o : offsets) {
            outlineRect.x = dstRect.x + o.x;
            outlineRect.y = dstRect.y + o.y;
            SDL_RenderTexture(renderer, cached.outline, nullptr, &outlineRect);
        }
    }

    // Draw the main text on top
    SDL_RenderTexture(renderer, cached.text, nullptr, &dstRect);
}

void JFLX::SDL3::TextRenderer::drawText(const std::string& text, float fontSize, float x, float y, int fontIndex_, const SDL_Color color, int orientation, bool outline, SDL_Renderer* rRenderer) const {
    TTF_Font* font = getFont(fontIndex_);
    JFLX::SDL3::TextRenderer::drawText(text, fontSize, x, y, font, color, orientation, outline, rRenderer);
}

void JFLX::SDL3::TextRenderer::drawText(const std::string& text, float fontSize, float x, float y, const std::string& fontName, const SDL_Color color, int orientation, bool outline, SDL_Renderer* rRenderer) const {
    TTF_Font* font = getFont(fontName);
    JFLX::SDL3::TextRenderer::drawText(text, fontSize, x, y, font, color, orientation, outline, rRenderer);
}

void JFLX::SDL3::TextRenderer::clearTextCache() {
    for (const auto& entry : textCache) {
        if (entry.second.text) SDL_DestroyTexture(entry.second.text);
        if (entry.second.outline) SDL_DestroyTexture(entry.second.outline);
    }
    textCache.clear();
}

void JFLX::SDL3::TextRenderer::cleanUp() {
    clearTextCache();

    defaultRenderer = nullptr;

    for (auto& font : fonts) {
        TTF_CloseFont(font);
    }
    fonts.clear();
    fontIndex.clear();
}
