/*
 * This file must only be included in exactly ONE .cpp file!
 */

#pragma once

/* ==================== IMPORTS ==================== */

/** Use callbacks instead of main(). */
#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#define MINIAUDIO_IMPLEMENTATION
#include <miniaudio.h>

#include "omni.h"

#ifdef OMNI_SCENE
#include "omni_scene.h"
#endif

/* ==================== WINDOW PROPERTIES ==================== */

namespace
{
/** Window configuration settings. */
struct WindowProperties
{
    /* ========== METADATA ========== */

    /** App name in metadata, can be null. */
    const char *appName = nullptr;
    /** App version in metadata, can be null. */
    const char *appVersion = nullptr;
    /** App ID in metadata, can be null. */
    const char *appIdentifier = nullptr;

    /* ========== INIT FLAGS ========== */

    /** The flags to pass into SDL_Init(), (VIDEO | GAMEPAD) are enabled automatically. */
    SDL_InitFlags initFlags = 0;

    /* ========== WINDOW ========== */

    /** The window title, can be null. */
    const char *title = nullptr;
    /** The window width. */
    int width = 640;
    /** The window height. */
    int height = 480;
    /** The window flags to pass into SDL_CreateWindowAndRenderer(), no flags by default. */
    SDL_WindowFlags windowFlags = 0;

    /* ========== COPYING & MOVING ========== */

    WindowProperties(const WindowProperties &) = delete;
    WindowProperties &operator=(const WindowProperties &) = delete;
    WindowProperties(WindowProperties &&) = delete;
    WindowProperties &operator=(WindowProperties &&) = delete;

    /* ========== OPERATORS ========== */

    /** Add to the init flags. */
    WindowProperties &operator|=(const SDL_InitFlags addInitFlags)
    {
        initFlags |= addInitFlags;
        return *this;
    }

    /** Add to the window flags. */
    WindowProperties &operator|=(const SDL_WindowFlags addWindowFlags)
    {
        windowFlags |= addWindowFlags;
        return *this;
    }
};
} // namespace

/* ==================== USER FUNCTIONS ==================== */

/** Provide the necessary properties to initialize an SDL3 window. */
static WindowProperties InitWindow(int argc, char *argv[]);

#ifdef OMNI_SCENE
/** Called once after the window is initialized, return the initial Scene, null to exit before running game loop. */
static Omni::Scene *Init(int argc, char *argv[]);

/** Called continuously before the current scene renders. */
static void PreRender(float dt);

/** Called continuously after the current scene renders. */
static void PostRender(float dt);
#else
/** Called once after the window is initialized, return false to exit before running game loop. */
static bool Init(int argc, char *argv[]);

/** Called continuously while the window shouldn't close, return false to exit game loop. */
static bool Render(float dt);
#endif

/** Called once in SDL3's AppQuit callback function. */
static void Dispose();

/* ==================== INTERNAL ==================== */

namespace
{
namespace internal
{
/** Prevents the user from altering and potentially breaking the app's state; static in SDL_AppInit. */
struct AppState
{
    SDL_Window *window = nullptr;
    SDL_Renderer *renderer = nullptr;
    ma_engine *soundEngine = nullptr;

    bool quit = false;
    bool keysPressed[SDL_SCANCODE_COUNT] = {};
    bool keysJustPressed[SDL_SCANCODE_COUNT] = {};
    bool keyPressed = false;

#ifdef OMNI_SCENE
    Omni::Scene *currentScene = nullptr;
    Omni::Scene *nextScene = nullptr;
#endif
};

/* ========== SDL_AppInit ========== */

SDL_Window *window = nullptr;
SDL_Renderer *renderer = nullptr;

/* ========== SDL_AppEvent ========== */

const bool *keysPressed = nullptr;
const bool *keysJustPressed = nullptr;

/* ========== SDL_AppIterate ========== */

float deltaTime = 0.0f;
Uint32 fps = 0;

/* ========== Misc. ========== */

Omni::Camera camera;
} // namespace internal
} // namespace

/* ==================== OMNI DEFINITIONS ==================== */

/* ========== WINDOW & RENDERER ========== */

inline SDL_Window *Omni::Window()
{
    return internal::window;
}

inline SDL_Renderer *Omni::Renderer()
{
    return internal::renderer;
}

/* ========== CAMERA ========== */

inline void Omni::RenderToCamera(const Omni::Camera *camera)
{
    Omni::Camera &cam = internal::camera;

    // Copy the provided camera's data into the internal camera and set the renderer's scale
    if (camera) {
        cam = *camera;
        SDL_SetRenderScale(internal::renderer, cam.zoom, cam.zoom);
        return;
    }

    // Reset the internal camera and set renderer scale back to default
    cam = {};
    SDL_SetRenderScale(internal::renderer, 1, 1);
}

inline bool Omni::RenderPoint(float x, float y)
{
    return SDL_RenderPoint(internal::renderer, x - internal::camera.x, y - internal::camera.y);
}

inline bool Omni::RenderPoints(const SDL_FPoint *points, int count)
{
    // Ensure points are available and the camera is set to a non-default position
    if (points && count > 0 && (internal::camera.x != 0 || internal::camera.y != 0)) {
        float x = internal::camera.x;
        float y = internal::camera.y;

        SDL_FPoint fpoint;
        for (int i = 0; i < count; i++) {
            const SDL_FPoint &point = points[i];
            fpoint.x = point.x - x;
            fpoint.y = point.y - y;

            // This is what SDL_RenderPoint() performs under the hood
            if (!SDL_RenderPoints(internal::renderer, &fpoint, 1))
                return false;
        }
        return true;
    }

    // Handles errors if no points provided and rendering with default camera position (0,0)
    return SDL_RenderPoints(internal::renderer, points, count);
}

inline bool Omni::RenderLine(float x1, float y1, float x2, float y2)
{
    float x = internal::camera.x;
    float y = internal::camera.y;
    return SDL_RenderLine(internal::renderer, x1 - x, y1 - y, x2 - x, y2 - y);
}

inline bool Omni::RenderLines(const SDL_FPoint *points, int count)
{
    // Ensure points are available and the camera is set to a non-default position
    if (points && count > 1 && (internal::camera.x != 0 || internal::camera.y != 0)) {
        float x = internal::camera.x;
        float y = internal::camera.y;

        // Create a camera-projected variant of each point
        SDL_FPoint *camPoints = new SDL_FPoint[count];
        for (int i = 0; i < count; i++) {
            const SDL_FPoint &point = points[i];
            SDL_FPoint &camPoint = camPoints[i];
            camPoint.x = point.x - x;
            camPoint.y = point.y - y;
        }

        // Render the camera-projected lines
        bool result = SDL_RenderLines(internal::renderer, camPoints, count);
        delete[] camPoints;
        return result;
    }

    // Handles errors if not enough points provided and rendering with default camera position (0,0)
    return SDL_RenderLines(internal::renderer, points, count);
}

inline bool Omni::RenderRect(SDL_FRect rect)
{
    rect.x -= internal::camera.x;
    rect.y -= internal::camera.y;
    return SDL_RenderRect(internal::renderer, &rect);
}

inline bool Omni::RenderRects(const SDL_FRect *rects, int count)
{
    // Ensure rects are available and the camera is set to a non-default position
    if (rects && count > 0 && (internal::camera.x != 0 || internal::camera.y != 0)) {
        // Camera data
        float x = internal::camera.x;
        float y = internal::camera.y;
        SDL_FRect camRect;

        // SDL_RenderRects() also iterates and calls SDL_RenderRect() for each rect.
        // For efficiency, we do the same but reuse camRect to apply the camera position.
        bool result = true;
        for (int i = 0; i < count; i++) {
            const SDL_FRect &rect = rects[i];
            camRect.x = rect.x - x;
            camRect.y = rect.y - y;
            camRect.w = rect.w;
            camRect.h = rect.h;

            if (!SDL_RenderRect(internal::renderer, &camRect))
                result = false;
        }

        return result;
    }

    // Handles errors if no rects provided and rendering with default camera position (0,0)
    return SDL_RenderRects(internal::renderer, rects, count);
}

/* ========== INPUTS ========== */

inline bool Omni::IsKeyPressed(SDL_Scancode key)
{
    return internal::keysPressed[key];
}

inline bool Omni::IsKeyPressed(SDL_Keycode key)
{
    return internal::keysPressed[SDL_GetScancodeFromKey(key, nullptr)];
}

inline bool Omni::IsKeyJustPressed(SDL_Scancode key)
{
    return internal::keysJustPressed[key];
}

inline bool Omni::IsKeyJustPressed(SDL_Keycode key)
{
    return internal::keysJustPressed[SDL_GetScancodeFromKey(key, nullptr)];
}

/* ========== GAME LOOP ========== */

inline float Omni::DeltaTime()
{
    return internal::deltaTime;
}

inline Uint32 Omni::FPS()
{
    return internal::fps;
}

/* ==================== SDL3 CALLBACK SETUP ==================== */

/* Runs once at startup. */
inline SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[])
{
    // Create the app state to hide from user
    static internal::AppState appState;
    *appstate = &appState;

    // Get the user's window configuration
    const WindowProperties properties = InitWindow(argc, argv);

    // Set the app metadata using the user's properties
    SDL_SetAppMetadata(properties.appName, properties.appVersion, properties.appIdentifier);

    // Set the initialization flags, VIDEO and GAMEPAD are always enabled
    if (!SDL_Init(properties.initFlags | SDL_INIT_VIDEO | SDL_INIT_GAMEPAD)) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't initialize SDL: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    // Create a window with the provided title, size, and flags; also create a renderer
    if (!SDL_CreateWindowAndRenderer(properties.title, properties.width, properties.height, properties.windowFlags,
                                     &appState.window, &appState.renderer)) {
        SDL_LogError(SDL_LOG_CATEGORY_VIDEO, "Couldn't create window/renderer: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    // Provide the window and renderer to allow correct functionality for Omni::Window() and Omni::Renderer()
    internal::window = appState.window;
    internal::renderer = appState.renderer;

    // Provide the input arrays to allow correct and safe functionality for Omni input getters
    internal::keysPressed = appState.keysPressed;
    internal::keysJustPressed = appState.keysJustPressed;

    // Use screen viewport as default
    SDL_SetRenderLogicalPresentation(appState.renderer, 0, 0, SDL_LOGICAL_PRESENTATION_DISABLED);

    // Initialize miniaudio engine
    appState.soundEngine = new ma_engine;
    if (ma_engine_init(nullptr, appState.soundEngine) != MA_SUCCESS) {
        delete appState.soundEngine;
        appState.soundEngine = nullptr;
        return SDL_APP_FAILURE;
    }

    // After SDL3, the window, and miniaudio are prepared, call the user's Init() function
#ifdef OMNI_SCENE
    Omni::Scene *&currentScene = appState.currentScene;
    currentScene = Init(argc, argv);
    if (currentScene)
        currentScene->init();
    appState.nextScene = currentScene;
    return currentScene ? SDL_APP_CONTINUE : SDL_APP_FAILURE;
#else
    return Init(argc, argv) ? SDL_APP_CONTINUE : SDL_APP_FAILURE;
#endif
}

/* This function runs when a new event (mouse input, keypresses, etc.) occurs. */
inline SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event)
{
    internal::AppState &appState = *(internal::AppState *)appstate;

    if (event->type == SDL_EVENT_QUIT) {
        appState.quit = true;
        return SDL_APP_CONTINUE;
    }

    if (event->type == SDL_EVENT_KEY_DOWN) {
        SDL_Scancode key = event->key.scancode;
        if (!appState.keysPressed[key]) {
            appState.keysPressed[key] = true;
            appState.keysJustPressed[key] = true;
            appState.keyPressed = true;
        }
    } else if (event->type == SDL_EVENT_KEY_UP) {
        SDL_Scancode key = event->key.scancode;
        appState.keysPressed[key] = false;
        appState.keysJustPressed[key] = false;
    }

    return SDL_APP_CONTINUE;
}

/* This function runs once per frame, and is the heart of the program. */
inline SDL_AppResult SDL_AppIterate(void *appstate)
{
    static Uint64 startDt = SDL_GetTicksNS();
    static Uint64 startFps = startDt;
    static Uint32 frames = 0;
    const Uint64 end = SDL_GetTicksNS();

    // Calculate delta time
    internal::deltaTime = (end - startDt) / 1e9f;
    startDt = end;

    // Calculate FPS every 100 ms
    if (end - startFps > 100000000) {
        internal::fps = frames * 10;
        startFps = end;
        frames = 0;
    }
    frames += 1;

    internal::AppState &appState = *(internal::AppState *)appstate;

    // Clear the window to black (automatic for the user)
    SDL_SetRenderDrawColor(appState.renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(appState.renderer);

#ifdef OMNI_SCENE
    Omni::Scene *&nextScene = appState.nextScene;
    if (!nextScene)
        return SDL_APP_SUCCESS;
    Omni::Scene *&currentScene = appState.currentScene;

    // Changing Scenes: Dispose and free current scene, initialize the next scene and set it as current
    if (currentScene != nextScene) {
        if (currentScene->dispose())
            delete currentScene;
        nextScene->init();
        currentScene = nextScene;
    }

    // Called here to ensure the current scene and next scene always match to free memory from both properly
    // This also allows init() to be called before dispose() outside the game loop
    if (appState.quit)
        return SDL_APP_SUCCESS;

    // Update the global render functions and the current scene, mark the next scene for any potential changes
    PreRender(internal::deltaTime);
    nextScene = currentScene->render(internal::deltaTime);
    PostRender(internal::deltaTime);
#else
    if (appState.quit || !Render(internal::deltaTime))
        return SDL_APP_SUCCESS;
#endif

    // Automatically draw what is currently in the renderer
    SDL_RenderPresent(appState.renderer);

    if (appState.keyPressed) {
        for (int i = 0; i < SDL_SCANCODE_COUNT; i++) {
            appState.keysJustPressed[i] = false;
        }
        appState.keyPressed = false;
    }

    return SDL_APP_CONTINUE;
}

/* This function runs once at shutdown; SDL will clean up the window/renderer for us. */
inline void SDL_AppQuit(void *appstate, SDL_AppResult result)
{
    internal::AppState &appState = *(internal::AppState *)appstate;
#ifdef OMNI_SCENE
    // Dispose remaining scene (if available)
    Omni::Scene *&currentScene = appState.currentScene;
    if (currentScene && currentScene->dispose()) {
        delete currentScene;
        currentScene = nullptr;
        appState.nextScene = nullptr;
    }
#endif
    // Since the sound engine loads last, Init() is called right after so Dispose() must be called
    ma_engine *&soundEngine = appState.soundEngine;
    if (soundEngine) {
        Dispose();
        ma_engine_uninit(soundEngine);
        delete soundEngine;
        soundEngine = nullptr;
    }
}
