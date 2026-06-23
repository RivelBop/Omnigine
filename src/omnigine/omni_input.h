#pragma once

#include <SDL3/SDL_keycode.h>
#include <SDL3/SDL_scancode.h>

namespace Omni
{
/* ==================== KEYBOARD ==================== */

/** True if key scancode is pressed. Uses physical key location, best for movement keys (WASD, etc.). */
[[nodiscard]] bool IsKeyPressed(SDL_Scancode key);

/** True if keycode is pressed. Uses virtual key mapping. */
[[nodiscard]] bool IsKeyPressed(SDL_Keycode key);

/** True for one frame if key scancode is pressed. Uses physical key location. */
[[nodiscard]] bool IsKeyJustPressed(SDL_Scancode key);

/** True for one frame if keycode is pressed. Uses virtual key mapping, best for specific key shortcuts (I for inventory, etc.). */
[[nodiscard]] bool IsKeyJustPressed(SDL_Keycode key);
} // namespace Omni