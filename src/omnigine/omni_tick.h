#pragma once

#define OMNI_TICK(clock, dt, rate, logic, alpha) \
    clock += dt;                                 \
    while (clock >= rate) {                      \
        logic();                                 \
        clock -= rate;                           \
    }                                            \
    alpha = clock / rate;