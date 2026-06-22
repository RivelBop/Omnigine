#pragma once

#include <miniaudio.h>

namespace Omni
{
/** Access to the miniaudio sound engine. */
[[nodiscard]] ma_engine *SoundEngine();
} // namespace Omni