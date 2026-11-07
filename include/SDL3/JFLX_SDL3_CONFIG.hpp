//
// Global window & renderer configuration for the JFLX SDL3 sub-library.
//
// SDL3 resources are managed centrally here so that components do not each
// have to be handed a window or renderer. Assign the active resources once
// via setWindow()/setRenderer(); components then fall back to g_window /
// g_renderer whenever they were not given one explicitly.
//
// The stored pointers are borrowed, never owned: whoever destroys the SDL
// window/renderer must clear it here first (setWindow(nullptr) /
// setRenderer(nullptr)) so the configuration cannot dangle.
//

#ifndef JFLX_SDL3_CONFIG_HPP
#define JFLX_SDL3_CONFIG_HPP

#include <SDL3/SDL.h>

namespace JFLX::SDL3::CONFIG {

    /// Globally active SDL3 window (borrowed; nullptr while unconfigured).
    extern SDL_Window* g_window;

    /// Globally active SDL3 renderer (borrowed; nullptr while unconfigured).
    extern SDL_Renderer* g_renderer;

    /**
     * @brief Assigns the globally active SDL3 window.
     * @param window Window to activate, or nullptr to clear the configuration.
     * @return true if a window is now active, false if it was cleared.
     */
    bool setWindow(SDL_Window* window);

    /**
     * @brief Assigns the globally active SDL3 renderer.
     * @param renderer Renderer to activate, or nullptr to clear the configuration.
     * @return true if a renderer is now active, false if it was cleared.
     */
    bool setRenderer(SDL_Renderer* renderer);

}

#endif // JFLX_SDL3_CONFIG_HPP
