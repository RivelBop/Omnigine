#include "omni_main.h"
#include "omni_tick.h"
#include <iostream>

void Init() {
}

static void Tick() {
    std::cout << "TICK\n";
}

bool Render(float dt) {
    static float clock = 0.0f;
    OMNI_TICK(clock, dt, 0.05f, Tick)

    BeginDrawing();
    EndDrawing();

    return true;
}

void Dispose() {
}
