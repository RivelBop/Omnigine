#pragma once

namespace Omni {
    /* ==================== WINDOW ==================== */

    // The default, ideal window width
    extern const int WIDTH;
    // The default, ideal window height
    extern const int HEIGHT;

    /* ==================== GAME LOOP ==================== */

    // Returns the time it took for the previous frame to complete and start the next frame
    float DeltaTime();
    // Returns frames per second
    int FPS();
}
