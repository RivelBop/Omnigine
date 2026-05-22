#pragma once

#define OMNI_TICK(clock, dt, rate, logic) clock += dt; \
    while (clock >= rate) { \
        logic(); \
        clock -= rate; \
    }