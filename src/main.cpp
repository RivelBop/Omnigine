#include "omnigine_main.h"
#include <iostream>

void Init() {
}

bool Render(float dt) {
    using namespace Omni;

    std::cout << "Delta Time: " << dt << '\n';
    BeginDrawing();
    EndDrawing();

    return true;
}

void Dispose() {
}
