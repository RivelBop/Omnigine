#pragma once

#include <SDL3/SDL_stdinc.h>

namespace Omni
{
/* ==================== GAME LOOP ==================== */

/** Returns the time it took for the previous frame to complete and start the next frame. */
[[nodiscard]] float DeltaTime();
/** Returns frames per second. */
[[nodiscard]] Uint32 FPS();
} // namespace Omni