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

    /** Calls SDL_SetRenderLogicalPresentation() for the specified SDL_Renderer. */
    inline bool SetViewport(SDL_Renderer *renderer, const int width, const int height, const Viewport viewport) {
        return SDL_SetRenderLogicalPresentation(renderer, width, height, viewport);
    }

    /* ==================== GAME LOOP ==================== */

    /** Returns the time it took for the previous frame to complete and start the next frame. */
    [[nodiscard]] float DeltaTime();
    /** Returns frames per second. */
    [[nodiscard]] int FPS();
}
