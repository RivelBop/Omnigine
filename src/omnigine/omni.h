#pragma once

#include "omni_input.h"
#include "omni_miniaudio.h"
#include "omni_render.h"
#include "omni_window.h"

namespace Omni
{
/* ==================== GAME LOOP ==================== */

/** Returns the time it took for the previous frame to complete and start the next frame. */
[[nodiscard]] float DeltaTime();
/** Returns frames per second. */
[[nodiscard]] Uint32 FPS();
} // namespace Omni
