//
// Created by lenny on 22.01.2026.
//

#ifndef JFLX_SDL3_TEXTRENDERER_HPP
#define JFLX_SDL3_TEXTRENDERER_HPP

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>

#include <string>
#include <unordered_map>
#include <vector>

namespace JFLX::SDL3 {

    /**
     * @brief A utility class for rendering TrueType fonts via SDL3_ttf.
     *
     * TextRenderer manages a collection of TTF_Font objects and provides
     * convenient methods for drawing text onto an SDL_Renderer. Fonts can
     * be referenced either by a numeric index (insertion order) or by a
     * user-defined name string.
     *
     * Rendered text textures are cached per (renderer, font, size, colour,
     * outline, string) so repeated per-frame drawText() calls do not
     * re-rasterise and re-upload the same text. Call clearTextCache() before
     * destroying an SDL_Renderer the cache may hold textures for.
     *
     * Typical usage:
     *   1. Construct with (or later assign) an SDL_Renderer*.
     *   2. Load one or more fonts via loadFont().
     *   3. Call drawText() each frame inside your render loop.
     *   4. Call cleanUp() (or let the destructor handle it) when done.
     *
     * @note TTF_Init() / TTF_Quit() must be called externally — this class
     *       does not manage the SDL_ttf subsystem lifetime.
     */
    class TextRenderer {
        /// A cache key captures everything that affects the rasterised result.
        /// Orientation and position only affect blitting, not rasterisation.
        struct TextCacheKey {
            SDL_Renderer* renderer = nullptr;
            TTF_Font* font = nullptr;
            int size = 0;
            SDL_Color color {0, 0, 0, 255};
            bool outline = false;
            std::string text;

            bool operator==(const TextCacheKey& other) const noexcept {
                return renderer == other.renderer && font == other.font &&
                       size == other.size && outline == other.outline &&
                       text == other.text &&
                       color.r == other.color.r && color.g == other.color.g &&
                       color.b == other.color.b && color.a == other.color.a;
            }
        };

        struct TextCacheKeyHash {
            size_t operator()(const TextCacheKey& key) const noexcept;
        };

        /// Cached GPU textures for one key: the text itself and, when the
        /// outline variant was requested, its black outline (may be null).
        struct CachedText {
            SDL_Texture* text = nullptr;
            SDL_Texture* outline = nullptr;
        };

        /// The renderer used by default when no explicit renderer is passed to drawText().
        SDL_Renderer* defaultRenderer = nullptr;

        /// Ordered list of loaded fonts. The position in this vector is the numeric index.
        std::vector<TTF_Font*> fonts = {};

        /// Maps a user-defined font name → index into the fonts vector,
        /// allowing fonts to be looked up by name instead of a raw integer.
        std::unordered_map<std::string, int> fontIndex = {};

        /// Rasterised text textures, reused across drawText() calls.
        /// mutable: drawText() is logically const — only the cache changes.
        mutable std::unordered_map<TextCacheKey, CachedText, TextCacheKeyHash> textCache = {};

    public:

        /**
         * @brief Default constructor. No renderer is set; call setDefaultRenderer() before drawing.
         */
        TextRenderer();

        /**
         * @brief Constructs a TextRenderer with a pre-assigned default renderer.
         * @param r A valid SDL_Renderer* that will be used when no renderer is
         *          explicitly passed to drawText().
         */
        TextRenderer(SDL_Renderer* r);

        /**
         * @brief Destructor. Automatically calls cleanUp() to free all loaded fonts.
         */
        ~TextRenderer();

        /**
         * @brief Assigns (or replaces) the default renderer used for drawing.
         *
         * Switching to a different renderer clears the text texture cache,
         * because cached textures belong to the previous renderer.
         *
         * @param renderer The SDL_Renderer* to use as the default.
         */
        void setDefaultRenderer(SDL_Renderer* renderer);

        /**
         * @brief Retrieves a font by its numeric index (insertion order).
         * @param index Zero-based index into the internal fonts vector.
         * @return Pointer to the TTF_Font, or nullptr if the index is out of range.
         */
        [[nodiscard]] TTF_Font* getFont(int index) const;

        /**
         * @brief Retrieves a font by its user-defined name.
         * @param fontName The name string supplied when the font was loaded via loadFont().
         * @return Pointer to the TTF_Font, or nullptr if no font with that name exists.
         */
        [[nodiscard]] TTF_Font* getFont(const std::string& fontName) const;

        /**
         * @brief Loads a TrueType font from disk and stores it internally.
         *
         * The font is opened at a base size of 32 pt; the actual render size
         * is set dynamically per drawText() call via TTF_SetFontSize().
         * The program exits (exit(1)) if the font cannot be loaded.
         *
         * @param pathToFont Filesystem path to the .ttf / .otf file.
         * @param fontName   Optional name to associate with this font so it can
         *                   be retrieved later via getFont(string). Pass an empty
         *                   string (the default) to skip name registration.
         */
        void loadFont(const std::string& pathToFont, const std::string& fontName = "");

        /**
         * @brief Measures the pixel size @p text would occupy at @p fontSize.
         *
         * Applies @p fontSize to the font first (same side effect as drawText()),
         * then queries SDL_ttf without touching any renderer.
         *
         * @param text     The UTF-8 string to measure.
         * @param fontSize Point size to measure at.
         * @param font     Pointer to the TTF_Font to use.
         * @param width    Output: rendered width in pixels.
         * @param height   Output: rendered height in pixels.
         * @return true on success; false if @p font is null or measuring fails
         *         (outputs are left untouched).
         */
        [[nodiscard]] bool measureText(const std::string& text, float fontSize,
                                       TTF_Font* font, float& width, float& height) const;

        /**
         * @brief Convenience overload — selects the font by numeric index.
         * @see measureText(string, float, TTF_Font*, float&, float&)
         * @return false if the index is out of range.
         */
        [[nodiscard]] bool measureText(const std::string& text, float fontSize,
                                       int fontIndex_, float& width, float& height) const;

        /**
         * @brief Convenience overload — selects the font by name.
         * @see measureText(string, float, TTF_Font*, float&, float&)
         * @return false if no font with that name exists.
         */
        [[nodiscard]] bool measureText(const std::string& text, float fontSize,
                                       const std::string& fontName, float& width, float& height) const;

        /**
         * @brief Closes and removes a single font by numeric index.
         *
         * Cached text rendered with this font is discarded. Indices after
         * @p index shift down by one, and fontIndex entries are re-mapped
         * accordingly.
         *
         * @param index Zero-based index into the internal fonts vector.
         * @return true if the font existed and was removed, false otherwise.
         */
        bool unloadFont(int index);

        /**
         * @brief Closes and removes a single font by name.
         * @param fontName Name supplied when the font was loaded via loadFont().
         * @return true if the font existed and was removed, false otherwise.
         */
        bool unloadFont(const std::string& fontName);

        /**
         * @brief Renders a UTF-8 text string at the given position.
         *
         * Text is rasterised by SDL_ttf into a surface, uploaded to a GPU
         * texture, and cached — subsequent calls with the same
         * (renderer, font, size, colour, outline, string) reuse the texture
         * without touching the font or the GPU upload path. If @p outline is
         * true, the text is rendered a second time in black and blitted eight
         * times (one per cardinal/diagonal direction) behind the main text to
         * produce a solid outline effect.
         *
         * @param text        The UTF-8 string to render.
         * @param fontSize    Point size to render the text at (applied via TTF_SetFontSize).
         * @param x           Horizontal anchor position in screen/renderer coordinates.
         * @param y           Vertical position of the top edge of the text.
         * @param font        Pointer to the TTF_Font to use.
         * @param color       RGBA fill colour of the text (default: opaque black).
         * @param orientation Horizontal anchor mode relative to @p x:
         *                      -1 → right-aligned  (text ends   at x)
         *                       0 → centred         (text centred on x)
         *                       1 → left-aligned    (text starts at x, default)
         * @param outline     If true, draws a black outline around the text.
         * @param rRenderer   Renderer to use for this call. Falls back to the
         *                    defaultRenderer when nullptr (the default).
         */
        void drawText(const std::string &text, float fontSize, float x, float y,
                      TTF_Font* font,
                      SDL_Color color = {0, 0, 0, 255},
                      int orientation = 1,
                      bool outline = false,
                      SDL_Renderer *rRenderer = nullptr) const;

        /**
         * @brief Convenience overload — selects the font by numeric index.
         * @see drawText(string, float, float, float, TTF_Font*, ...)
         * @param fontIndex_ Zero-based index of the font (insertion order).
         */
        void drawText(const std::string &text, float fontSize, float x, float y,
                      int fontIndex_,
                      SDL_Color color = {0, 0, 0, 255},
                      int orientation = 1,
                      bool outline = false,
                      SDL_Renderer *rRenderer = nullptr) const;

        /**
         * @brief Convenience overload — selects the font by name.
         * @see drawText(string, float, float, float, TTF_Font*, ...)
         * @param fontName Name supplied when the font was loaded via loadFont().
         */
        void drawText(const std::string &text, float fontSize, float x, float y,
                      const std::string &fontName,
                      SDL_Color color = {0, 0, 0, 255},
                      int orientation = 1,
                      bool outline = false,
                      SDL_Renderer *rRenderer = nullptr) const;

        /**
         * @brief Destroys every cached text texture and empties the cache.
         *
         * Call this before destroying an SDL_Renderer this class may have
         * cached textures for — SDL_DestroyRenderer() frees renderer
         * resources, and the cache must not outlive them.
         * Safe to call multiple times; the cache is repopulated on demand.
         */
        void clearTextCache();

        /**
         * @brief Releases all loaded fonts, the text cache, and resets internal state.
         *
         * All TTF_Font pointers are closed via TTF_CloseFont(), cached text
         * textures are destroyed, and both the fonts vector and the fontIndex
         * map are cleared. The defaultRenderer pointer is set to nullptr
         * (the renderer itself is NOT destroyed).
         * Safe to call multiple times.
         */
        void cleanUp();
    };
}


#endif //JFLX_SDL3_TEXTRENDERER_HPP
