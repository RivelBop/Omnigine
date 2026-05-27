#include "omnigine/omni_main.h"

void Init() {
}

bool Render(float dt) {
    BeginDrawing();
        ClearBackground(BLACK);
        DrawText("Hello World!", 0, 0, 52, WHITE);
    EndDrawing();
    return true;
}

void Dispose() {
}
