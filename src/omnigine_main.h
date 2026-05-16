#pragma once

/* ==================== IMPORTS ==================== */

#include "omnigine.h"

#if defined(PLATFORM_WEB)
#include <emscripten/emscripten.h>
#endif

/* ==================== WINDOW PROPERTIES ==================== */

// Window Aspect Ratio
#ifndef OMNI_ASPECT_RATIO
#define OMNI_ASPECT_RATIO 16/9
#endif

// Window Height
#ifndef OMNI_HEIGHT
#define OMNI_HEIGHT 720
#endif

// Window Width
#ifndef OMNI_WIDTH
#define OMNI_WIDTH (OMNI_HEIGHT * OMNI_ASPECT_RATIO)
#endif

// Define Size Constants
const int Omni::WIDTH  = OMNI_WIDTH;
const int Omni::HEIGHT = OMNI_HEIGHT;

/* ==================== USER FUNCTIONS ==================== */

void Init();

void Render();

void Dispose();

/* ==================== WINDOW SETUP ==================== */

int main() {
    using namespace Omni;

    // Initialization
    InitWindow(WIDTH, HEIGHT, "Omnigine - Base Title");
    Init();

    // Game Loop
#if defined(PLATFORM_WEB)
    emscripten_set_main_loop(Render, 0, 1);
#else
    while (!WindowShouldClose()) {
        Render();
    }
#endif

    // De-Initialization
    Dispose();
    CloseWindow();
}
