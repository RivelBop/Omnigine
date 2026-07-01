#pragma once

#include <SDL3/SDL_rect.h>

#define OMNI_TICK(clock, dt, rate, logic, alpha) \
    clock += dt;                                 \
    while (clock >= rate) {                      \
        logic();                                 \
        clock -= rate;                           \
    }                                            \
    alpha = clock / rate;

namespace Omni
{

inline float Lerp(float start, float end, float alpha)
{
    return start + (end - start) * alpha;
}

inline SDL_FPoint Lerp(SDL_FPoint start, const SDL_FPoint &end, float alpha)
{
    start.x += (end.x - start.x) * alpha;
    start.y += (end.y - start.y) * alpha;
    return start;
}

} // namespace Omni