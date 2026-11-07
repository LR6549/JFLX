//
// Definitions of the global SDL3 window/renderer configuration.
//

#include <SDL3/JFLX_SDL3_CONFIG.hpp>

namespace JFLX::SDL3::CONFIG {

    SDL_Window* g_window = nullptr;
    SDL_Renderer* g_renderer = nullptr;

    bool setWindow(SDL_Window* window) {
        g_window = window;
        return window != nullptr;
    }

    bool setRenderer(SDL_Renderer* renderer) {
        g_renderer = renderer;
        return renderer != nullptr;
    }

}
