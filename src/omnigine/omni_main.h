#pragma once

/* ==================== IMPORTS ==================== */

#include "raylib.h"
#include "omni.h"

#ifdef OMNI_SCENE
#include "omnigine_scene.h"
#endif

#ifdef PLATFORM_WEB
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

const int Omni::WIDTH  = OMNI_WIDTH;
const int Omni::HEIGHT = OMNI_HEIGHT;

/* ==================== USER FUNCTIONS ==================== */

#ifdef OMNI_SCENE
// Called once after the window is initialized, return the initial Scene
static Omni::Scene* Init();

// Called continuously before the current scene renders
static void PreRender(float dt);

// Called continuously after the current scene renders
static void PostRender(float dt);
#else
// Called once after the window is initialized
static void Init();

// Called continuously while the window shouldn't close, return false when exiting game loop in-game
static bool Render(float dt);
#endif

// Called once right before the window closes
static void Dispose();

/* ==================== WINDOW SETUP ==================== */

// Not inline since there can only be one translation unit with this header
int main() {
    // Initialization
    InitWindow(Omni::WIDTH, Omni::HEIGHT, "Omnigine - Base Title");
#ifdef OMNI_SCENE
    // Static for stateless lambda for web
    static Scene *currentScene = Init();
    if (currentScene)
        currentScene->init();
    static Scene *nextScene = currentScene;
#else
    Init();
#endif

    // Game Loop
#ifdef PLATFORM_WEB
#ifdef OMNI_SCENE
    emscripten_set_main_loop([]() {
        // If there is no next scene, the game loop is considered terminated
        if (!nextScene) {
            emscripten_cancel_main_loop();

            // De-Initialization
            Dispose();
            CloseWindow();
            return;
        }

        // Changing Scenes: The current scene is already disposed, initialize the next scene and set it as current
        if (currentScene != nextScene) {
            nextScene->init();
            currentScene = nextScene;
        }

        // Update the global render functions and the current scene, mark the next scene for any potential changes
        const float dt = GetFrameTime();
        PreRender(dt);
        nextScene = currentScene->render(dt);
        PostRender(dt);

        // On scene change, if marked to be freed via dispose(), free the current scene from memory
        if (currentScene != nextScene && currentScene->dispose()) {
            delete currentScene;
            currentScene = nullptr;
        }
    }, 0, 1);
#else
    emscripten_set_main_loop([]() {
        if (!Render(GetFrameTime())) {
            emscripten_cancel_main_loop();

            // De-Initialization
            Dispose();
            CloseWindow();
        }
    }, 0, 1);
#endif
#else // PLATFORM_WEB
#ifdef OMNI_SCENE
    while (nextScene) {
        // Changing Scenes: Dispose and free current scene, initialize the next scene and set it as current
        if (currentScene != nextScene) {
            if (currentScene->dispose())
                delete currentScene;
            nextScene->init();
            currentScene = nextScene;
        }

        // Called here to ensure the current scene and next scene always match to free memory from both properly
        // This also allows init() to be called before dispose() outside the game loop
        if (WindowShouldClose()) break;

        // Update the global render functions and the current scene, mark the next scene for any potential changes
        const float dt = GetFrameTime();
        PreRender(dt);
        nextScene = currentScene->render(dt);
        PostRender(dt);
    }

    // After game loop, dispose remaining scene (if available)
    if (currentScene && currentScene->dispose()) {
        delete currentScene;
        currentScene = nullptr;
        nextScene = nullptr;
    }
#else
    // Standard Game Loop
    while (!WindowShouldClose() && Render(GetFrameTime()));
#endif // OMNI_SCENE
#endif // PLATFORM_WEB

    // De-Initialization
    Dispose();
    CloseWindow();
}
