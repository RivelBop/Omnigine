#pragma once

#include <SDL3/SDL_render.h>
#include <SDL3/SDL_video.h>

namespace Omni {
    /* ==================== WINDOW AND RENDERING ==================== */

    using Viewport = SDL_RendererLogicalPresentation;
    inline constexpr Viewport SCREEN_VIEWPORT        = SDL_LOGICAL_PRESENTATION_DISABLED;
    inline constexpr Viewport STRETCH_VIEWPORT       = SDL_LOGICAL_PRESENTATION_STRETCH;
    inline constexpr Viewport FIT_VIEWPORT           = SDL_LOGICAL_PRESENTATION_LETTERBOX;
    inline constexpr Viewport FILL_VIEWPORT          = SDL_LOGICAL_PRESENTATION_OVERSCAN;
    inline constexpr Viewport PIXEL_PERFECT_VIEWPORT = SDL_LOGICAL_PRESENTATION_INTEGER_SCALE;

    /** Access to the window pointer. */
    [[nodiscard]] SDL_Window *Window();
    /** Access to the renderer pointer. */
    [[nodiscard]] SDL_Renderer *Renderer();

    /** Calls SDL_SetRenderLogicalPresentation() for the global Renderer(). */
    inline bool SetViewport(const int width, const int height, const Viewport viewport) {
        return SDL_SetRenderLogicalPresentation(Renderer(), width, height, viewport);
    }

    /* ==================== INPUTS ==================== */

    /** True if key scancode is pressed. Uses physical key location, best for movement keys (WASD, etc.). */
    [[nodiscard]] bool IsKeyPressed(SDL_Scancode key);

    /** True if keycode is pressed. Uses virtual key mapping. */
    [[nodiscard]] bool IsKeyPressed(SDL_Keycode key);

    /** True for one frame if key scancode is pressed. Uses physical key location. */
    [[nodiscard]] bool IsKeyJustPressed(SDL_Scancode key);

    /** True for one frame if keycode is pressed. Uses virtual key mapping, best for specific key shortcuts (I for inventory, etc.). */
    [[nodiscard]] bool IsKeyJustPressed(SDL_Keycode key);

    /* ==================== GAME LOOP ==================== */

    /** Returns the time it took for the previous frame to complete and start the next frame. */
    [[nodiscard]] float DeltaTime();
    /** Returns frames per second. */
    [[nodiscard]] Uint32 FPS();
}
