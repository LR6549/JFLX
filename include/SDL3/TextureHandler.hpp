//
// Created by lenny on 21.01.2026.
//

#ifndef JFLX_SDL3_TEXTUREHANDLER_HPP
#define JFLX_SDL3_TEXTUREHANDLER_HPP

#define JFLX_SDL3_TEXTUREHANDLER

#include <SDL3/SDL.h>
#include <SDL3/JFLX_SDL3_CONFIG.hpp>

#include <string>
#include <unordered_map>
#include <vector>

namespace JFLX::SDL3 {

    /**
     * @brief Controls which point of the texture is placed at (x, y).
     *
     *  CENTERED     – texture centre          → (x, y)
     *  TOPLEFT      – top-left corner         → (x, y)  [SDL default]
     *  TOPRIGHT     – top-right corner        → (x, y)
     *  BOTTOMLEFT   – bottom-left corner      → (x, y)
     *  BOTTOMRIGHT  – bottom-right corner     → (x, y)
     *
     * Rotation (if non-zero) always pivots around the anchor point.
     */
    enum class renderMode {
        JFLX_RENDER_CENTERED,
        JFLX_RENDER_TOP_CENTERED,
        JFLX_RENDER_TOP_LEFT,
        JFLX_RENDER_LEFT_CENTERED,
        JFLX_RENDER_TOP_RIGHT,
        JFLX_RENDER_RIGHT_CENTERED,
        JFLX_RENDER_BOTTOM_CENTERED,
        JFLX_RENDER_BOTTOM_LEFT,
        JFLX_RENDER_BOTTOM_RIGHT
    };

    /**
     * @brief Manages a collection of SDL_Texture* objects loaded from a folder.
     *
     * Textures are stored in a flat, ordered list ("layers"). Each layer is
     * identified both by its integer index and by the stem of the source file
     * (e.g. "player" for "player.png"). This makes sprite-sheet–style layer
     * access and name-based look-up equally convenient.
     *
     * Lifetime:  The handler does NOT own the SDL_Renderer; it only borrows it.
     *            It DOES own every SDL_Texture* it creates and destroys them in
     *            cleanup() / the destructor.
     */
    class TextureHandler {
    private:
        /// Per-instance renderer override, borrowed and never destroyed by this
        /// class. nullptr means "use the globally configured renderer".
        SDL_Renderer* renderer = nullptr;

        /// The renderer actually used: the per-instance override when one was
        /// given, otherwise JFLX::SDL3::CONFIG::g_renderer.
        [[nodiscard]] SDL_Renderer* activeRenderer() const noexcept {
            return renderer ? renderer : CONFIG::g_renderer;
        }

        /// Flat list of loaded textures. The position in this vector is the "layer index".
        /// Index 0 = first file loaded, 1 = second, etc.
        std::vector<SDL_Texture*> textures;

        /// Maps a texture's name (file stem, e.g. "background") to its layer index.
        /// Populated in lockstep with the textures vector so both stay in sync.
        std::unordered_map<std::string, int> textureLayers;

    public:
        /// Default constructor – draws with the globally configured renderer
        /// (JFLX::SDL3::CONFIG) unless one is assigned via setRenderer().
        TextureHandler() = default;

        /// Preferred constructor: immediately binds a valid renderer, which
        /// takes precedence over the globally configured one.
        explicit TextureHandler(SDL_Renderer* renderer) : renderer(renderer) {};

        /**
         * @brief Replaces the current renderer reference.
         * @param r  New renderer pointer; silently ignored if null to prevent
         *           accidentally clearing a working renderer.
         */
        void setRenderer(SDL_Renderer* r) { if (r != nullptr) this->renderer = r; }

        /**
         * @brief Loads every image file inside @p folderPath as a separate texture layer.
         *
         * Files are loaded in directory-iteration order. The layer index assigned to each
         * texture corresponds to its position in that order. The texture name (used as the
         * key in textureLayers) is the file's stem without extension.
         *
         * @param folderPath  Path to the folder containing image files.
         * @param scaling Change the texture scaling mode, default is nearest
         * @return true  if at least one texture was loaded successfully.
         * @return false if the folder is empty, inaccessible, or no file could be loaded.
         */
        bool loadTextureFolder(const std::string& folderPath, SDL_ScaleMode scaling = SDL_SCALEMODE_NEAREST);

        /**
         * @brief Slices a tilesheet image into a grid of individual tile textures.
         *
         * The image at @p imagePath is divided into @p tileWidth × @p tileHeight
         * cells (left to right, top to bottom). Each tile becomes its own layer,
         * named "<image stem>_<n>" with n counting row-major from 0 — e.g.
         * "tiles_0", "tiles_1", … for "tiles.png". The first tile's layer index
         * can be retrieved with getTextureLayer("<image stem>_0"), so tile id T
         * of the sheet maps to layer (firstLayer + T).
         *
         * Unlike loadTextureFolder(), existing layers are kept — tilesheets can
         * be added alongside previously loaded textures.
         *
         * Supported image formats: png, jpg, bmp, gif.
         *
         * @param imagePath   Path to the tilesheet image file.
         * @param tileWidth   Tile width in pixels (> 0).
         * @param tileHeight  Tile height in pixels (> 0).
         * @param scaling     Texture scaling mode, default is nearest (pixel art friendly).
         * @return true  if at least one tile was sliced successfully.
         * @return false if the file is missing/invalid, the renderer is unset,
         *               or the image is smaller than a single tile.
         */
        bool loadTileset(const std::string& imagePath, int tileWidth, int tileHeight,
                         SDL_ScaleMode scaling = SDL_SCALEMODE_NEAREST);

        /**
         * @brief Checks whether a texture with the given name has been loaded.
         * @param name  Texture name (file stem, e.g. "player").
         * @return true if the name exists in textureLayers.
         */
        bool exists(const std::string& name) const;

        /**
         * @brief Returns the total number of loaded texture layers.
         * @note [[nodiscard]] because ignoring the return value is almost always a bug.
         */
        [[nodiscard]] int layers() const {
            return static_cast<int>(textures.size());
        }

        /**
         * @brief Looks up the layer index for a named texture.
         * @param name  Texture name (file stem).
         * @return Layer index ≥ 0 on success, or -1 if the name is not found.
         */
        int getTextureLayer(const std::string& name) const;

        /**
         * @brief Direct access to the raw SDL_Texture* at the given layer.
         * @param layer  Zero-based layer index.
         * @return Pointer to the texture, or nullptr if @p layer is out of range.
         */
        SDL_Texture* getTexture(int layer) const;

        /**
         * @brief Destroys the texture at the given layer and removes it.
         *
         * Layer indices after @p layer shift down by one and textureLayers
         * entries are re-mapped accordingly. Raw SDL_Texture* pointers obtained
         * via getTexture() for other layers stay valid.
         *
         * @param layer Zero-based layer index.
         * @return true if the layer existed and was removed, false otherwise.
         */
        bool unloadTexture(int layer);

        /**
         * @brief Destroys a named texture and removes its layer.
         * @param name  Texture name (file stem).
         * @return true if the texture existed and was removed, false otherwise.
         */
        bool unloadTexture(const std::string& name);

        // ------------------------------------------------------------------ //
        //  Render overloads
        //
        //  (x, y) is the anchor point whose meaning is determined by @p mode:
        //
        //  Rotation always pivots around the anchor point.
        // ------------------------------------------------------------------ //

        /**
         * @brief Renders the texture at the given layer index.
         *
         * @param layer     Zero-based layer index of the texture to render.
         * @param x         Anchor X position in renderer coordinates.
         * @param y         Anchor Y position in renderer coordinates.
         * @param mode      Which corner / centre of the texture is placed at (x, y).
         * @param scaleX    Horizontal scale factor (1.0 = original width).
         * @param scaleY    Vertical scale factor   (1.0 = original height).
         * @param rotation  Clockwise rotation in degrees around the anchor point.
         * @param flip      SDL_FLIP_NONE / SDL_FLIP_HORIZONTAL / SDL_FLIP_VERTICAL.
         * @param color     Colour & alpha modulation applied before rendering
         *                  (white + full alpha = no modulation).
         * @return true on success, false if the layer is invalid or rendering fails.
         */
        bool renderTexture(int layer,
                           float x, float y,
                           renderMode mode      = renderMode::JFLX_RENDER_CENTERED,
                           float scaleX         = 1.f,
                           float scaleY         = 1.f,
                           double rotation      = 0.0,
                           SDL_FlipMode flip    = SDL_FLIP_NONE,
                           SDL_Color color      = {255, 255, 255, 255}) const;

        /**
         * @brief Renders the texture identified by name (convenience overload).
         *
         * Internally resolves the name to a layer index via textureLayers and
         * delegates to the index-based overload above.
         *
         * @param name  Texture name (file stem, e.g. "player").
         * @return true on success, false if the name is not found or rendering fails.
         */
        bool renderTexture(const std::string& name,
                           float x, float y,
                           renderMode mode      = renderMode::JFLX_RENDER_CENTERED,
                           float scaleX         = 1.f,
                           float scaleY         = 1.f,
                           double rotation      = 0.0,
                           SDL_FlipMode flip    = SDL_FLIP_NONE,
                           SDL_Color color      = {255, 255, 255, 255}) const;

        /**
         * @brief Destroys all loaded textures and clears the internal containers.
         *
         * Safe to call multiple times. After cleanup() the handler is in the same
         * state as after default construction (renderer reference is kept).
         */
        void cleanup();

        /// Destructor delegates to cleanup() to ensure all SDL_Texture* are freed.
        ~TextureHandler() { cleanup(); }
    };

}

#endif
