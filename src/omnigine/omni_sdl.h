#pragma once

#include <SDL3/SDL.h>

namespace Omni
{
/** Access to the SDL window. */
[[nodiscard]] SDL_Window *Window();
/** Access to the SDL renderer. */
[[nodiscard]] SDL_Renderer *Renderer();
} // namespace Omni