//
// Created by lenny on 21.01.2026.
//

#include <JFLX/SDL3/TextureHandler.hpp>

#include <JFLX/logging.hpp>

#include <SDL3_image/SDL_image.h>

#include <filesystem>
#include <string>

namespace fs = std::filesystem;

namespace {

    /// Extensions loadTextureFolder() picks up (matches IMG_Load's formats).
    bool isImageExtension(const fs::path& extension) {
        return extension == ".png" || extension == ".jpg" ||
               extension == ".bmp" || extension == ".gif";
    }

} // namespace

// Only Supports png, jpg, bmp and gif
bool JFLX::SDL3::TextureHandler::loadTextureFolder(const std::string& folderPath, SDL_ScaleMode scaling) {
    if (!renderer) {
        JFLX::log("Texture folder load failed, no renderer set: ", folderPath, JFLX::LOGTYPE::JFLX_ERROR);
        return false;
    }

    cleanup();

    if (!fs::exists(folderPath)) {
        JFLX::log("Texture folder does not exist: ", folderPath, JFLX::LOGTYPE::JFLX_INFO);
        return false;
    }

    for (const auto& dir : fs::recursive_directory_iterator(folderPath)) {
        if (!dir.is_regular_file() || !isImageExtension(dir.path().extension())) continue;

        const std::string tempPath    = dir.path().string();
        const std::string tempNameExt = dir.path().filename().string();   // "player.png"
        const std::string tempName    = dir.path().stem().string();       // "player"

        JFLX::log("Texture: ", (tempNameExt + " in " + tempPath), JFLX::LOGTYPE::JFLX_INFO);

        SDL_Texture* tex = IMG_LoadTexture(renderer, tempPath.c_str());
        if (!tex) {
            JFLX::log("Failed to load texture: ", SDL_GetError(), JFLX::LOGTYPE::JFLX_ERROR);
            continue;
        }

        SDL_SetTextureScaleMode(tex, scaling);

        textureLayers[tempName] = static_cast<int>(textures.size());
        textures.push_back(tex);
        JFLX::log("Loaded Texture: ", tempName, JFLX::LOGTYPE::JFLX_SUCCESS);
    }

    return !textures.empty();
}

bool JFLX::SDL3::TextureHandler::loadTileset(const std::string& imagePath, int tileWidth,
                                             int tileHeight, SDL_ScaleMode scaling) {
    if (!renderer) {
        JFLX::log("loadTileset failed, no renderer set: ", imagePath, JFLX::LOGTYPE::JFLX_ERROR);
        return false;
    }
    if (tileWidth <= 0 || tileHeight <= 0) {
        JFLX::log("loadTileset: invalid tile size: ",
                  std::to_string(tileWidth) + "x" + std::to_string(tileHeight),
                  JFLX::LOGTYPE::JFLX_ERROR);
        return false;
    }
    if (!fs::exists(imagePath)) {
        JFLX::log("loadTileset: tilesheet not found: ", imagePath, JFLX::LOGTYPE::JFLX_ERROR);
        return false;
    }

    SDL_Surface* sheet = IMG_Load(imagePath.c_str());
    if (!sheet) {
        JFLX::log("loadTileset: failed to load tilesheet: ", SDL_GetError(), JFLX::LOGTYPE::JFLX_ERROR);
        return false;
    }

    // Normalise once so every tile view below is plain 32-bit RGBA.
    SDL_Surface* rgba = SDL_ConvertSurface(sheet, SDL_PIXELFORMAT_RGBA32);
    SDL_DestroySurface(sheet);
    if (!rgba) {
        JFLX::log("loadTileset: failed to convert tilesheet: ", SDL_GetError(), JFLX::LOGTYPE::JFLX_ERROR);
        return false;
    }

    const int cols = rgba->w / tileWidth;
    const int rows = rgba->h / tileHeight;
    if (cols == 0 || rows == 0) {
        JFLX::log("loadTileset: image smaller than one tile: ", imagePath, JFLX::LOGTYPE::JFLX_ERROR);
        SDL_DestroySurface(rgba);
        return false;
    }

    const std::string stem = fs::path(imagePath).stem().string();
    int loaded = 0;

    for (int row = 0; row < rows; ++row) {
        for (int col = 0; col < cols; ++col) {
            // Zero-copy view onto this tile's pixels inside the sheet; the
            // texture creation below copies just these pixels to the GPU.
            auto* pixels = static_cast<Uint8*>(rgba->pixels)
                    + static_cast<size_t>(row * tileHeight) * static_cast<size_t>(rgba->pitch)
                    + static_cast<size_t>(col * tileWidth) * 4u;

            SDL_Surface* tileView = SDL_CreateSurfaceFrom(tileWidth, tileHeight,
                                                          SDL_PIXELFORMAT_RGBA32,
                                                          pixels, rgba->pitch);
            if (!tileView) {
                JFLX::log("loadTileset: failed to view tile: ", SDL_GetError(), JFLX::LOGTYPE::JFLX_ERROR);
                continue;
            }

            SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, tileView);
            SDL_DestroySurface(tileView);
            if (!tex) {
                JFLX::log("loadTileset: failed to create tile texture: ", SDL_GetError(), JFLX::LOGTYPE::JFLX_ERROR);
                continue;
            }

            SDL_SetTextureScaleMode(tex, scaling);

            // Grid-position naming: "stem_<row*cols+col>".
            const std::string tileName = stem + "_" + std::to_string(row * cols + col);
            textureLayers[tileName] = static_cast<int>(textures.size());
            textures.push_back(tex);
            ++loaded;
        }
    }

    SDL_DestroySurface(rgba);

    if (loaded == 0) {
        JFLX::log("loadTileset: no tiles created from: ", imagePath, JFLX::LOGTYPE::JFLX_WARNING);
        return false;
    }

    JFLX::log("Sliced tilesheet: ", stem + " (" + std::to_string(loaded) + " tiles)",
              JFLX::LOGTYPE::JFLX_SUCCESS);
    return true;
}

bool JFLX::SDL3::TextureHandler::exists(const std::string& name) const {
    return textureLayers.contains(name);
}

int JFLX::SDL3::TextureHandler::getTextureLayer(const std::string& name) const {
    auto it = textureLayers.find(name);
    return (it == textureLayers.end()) ? -1 : it->second;
}

SDL_Texture* JFLX::SDL3::TextureHandler::getTexture(int layer) const {
    if (layer < 0 || layer >= static_cast<int>(textures.size())) {
        return nullptr;
    }
    return textures[layer];
}

bool JFLX::SDL3::TextureHandler::unloadTexture(int layer) {
    if (layer < 0 || layer >= static_cast<int>(textures.size())) {
        return false;
    }

    SDL_DestroyTexture(textures[layer]);
    textures.erase(textures.begin() + layer);

    // Re-map name → layer entries: drop the removed name, shift later ones.
    for (auto it = textureLayers.begin(); it != textureLayers.end(); ) {
        if (it->second == layer) {
            it = textureLayers.erase(it);
        } else {
            if (it->second > layer) --it->second;
            ++it;
        }
    }
    return true;
}

bool JFLX::SDL3::TextureHandler::unloadTexture(const std::string& name) {
    const int layer = getTextureLayer(name);
    if (layer == -1) {
        return false;
    }
    return unloadTexture(layer);
}

bool JFLX::SDL3::TextureHandler::renderTexture(int layer, float x, float y, renderMode mode, float scaleX, float scaleY, double rotation, SDL_FlipMode flip, SDL_Color color) const {
    SDL_Texture* tex = getTexture(layer);
    if (!tex) {
        JFLX::log("renderTexture: invalid layer: ", std::to_string(layer), JFLX::LOGTYPE::JFLX_ERROR);
        return false;
    }

    const float w = static_cast<float>(tex->w) * scaleX;
    const float h = static_cast<float>(tex->h) * scaleY;

    float anchorOffX = 0.f;
    float anchorOffY = 0.f;

    switch (mode) {
        case renderMode::JFLX_RENDER_CENTERED:
            anchorOffX = w / 2.f;   anchorOffY = h / 2.f;  break;
        case renderMode::JFLX_RENDER_TOP_CENTERED:
            anchorOffX = w / 2.f;   anchorOffY = 0.f;       break;
        case renderMode::JFLX_RENDER_TOP_LEFT:
            anchorOffX = 0.f;       anchorOffY = 0.f;       break;
        case renderMode::JFLX_RENDER_LEFT_CENTERED:
            anchorOffX = 0.f;       anchorOffY = h / 2.f;   break;
        case renderMode::JFLX_RENDER_TOP_RIGHT:
            anchorOffX = w;         anchorOffY = 0.f;       break;
        case renderMode::JFLX_RENDER_RIGHT_CENTERED:
            anchorOffX = w;         anchorOffY = h / 2.f;   break;
        case renderMode::JFLX_RENDER_BOTTOM_CENTERED:
            anchorOffX = w / 2.f;   anchorOffY = h;         break;
        case renderMode::JFLX_RENDER_BOTTOM_LEFT:
            anchorOffX = 0.f;       anchorOffY = h;         break;
        case renderMode::JFLX_RENDER_BOTTOM_RIGHT:
            anchorOffX = w;         anchorOffY = h;         break;
    }

    SDL_FRect dstRect {
        x - anchorOffX,
        y - anchorOffY,
        w,
        h
    };

    SDL_FPoint center { anchorOffX, anchorOffY };

    SDL_SetTextureColorMod(tex, color.r, color.g, color.b);
    SDL_SetTextureAlphaMod(tex, color.a);

    SDL_RenderTextureRotated(renderer, tex, nullptr, &dstRect,
                             rotation, &center, flip);

    SDL_SetTextureColorMod(tex, 255, 255, 255);
    SDL_SetTextureAlphaMod(tex, 255);

    return true;
}

bool JFLX::SDL3::TextureHandler::renderTexture(const std::string& name, float x, float y, renderMode mode, float scaleX, float scaleY, double rotation, SDL_FlipMode flip, SDL_Color color) const {
    const int layer = getTextureLayer(name);
    if (layer == -1) {
        JFLX::log("renderTexture: unknown texture: ", name, JFLX::LOGTYPE::JFLX_ERROR);
        return false;
    }
    return renderTexture(layer, x, y, mode, scaleX, scaleY, rotation, flip, color);
}

void JFLX::SDL3::TextureHandler::cleanup() {
    for (SDL_Texture* tex : textures) {
        SDL_DestroyTexture(tex);
    }
    textures.clear();
    textureLayers.clear();
}
