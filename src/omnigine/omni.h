#pragma once

#include <SDL3/SDL_render.h>
#include <SDL3/SDL_video.h>

namespace Omni {
    /* ==================== WINDOW ==================== */

    // The default, ideal window width
    extern const int WIDTH;
    // The default, ideal window height
    extern const int HEIGHT;

    // Access to the window pointer
    SDL_Window *Window();
    // Access to the renderer pointer
    SDL_Renderer *Renderer();

    /* ==================== GAME LOOP ==================== */

    // Returns the time it took for the previous frame to complete and start the next frame
    float DeltaTime();
    // Returns frames per second
    int FPS();
}
