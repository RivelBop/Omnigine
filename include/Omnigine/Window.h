#pragma once

#include <SDL3/SDL_video.h>

namespace Omni
{
/** Access to the SDL window. */
[[nodiscard]] SDL_Window *Window();
} // namespace Omni