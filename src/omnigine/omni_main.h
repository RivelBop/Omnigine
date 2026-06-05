/*
 * This file must only be included in exactly ONE .cpp file!
 */

#pragma once

/* ==================== IMPORTS ==================== */

/** Use callbacks instead of main(). */
#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include "omni.h"

#ifdef OMNI_SCENE
#include "omni_scene.h"
#endif

/* ==================== WINDOW PROPERTIES ==================== */

namespace {
    /** Window configuration settings. */
    struct WindowProperties {
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
        WindowProperties &operator|=(const SDL_InitFlags addInitFlags) {
            initFlags |= addInitFlags;
            return *this;
        }

        /** Add to the window flags. */
        WindowProperties &operator|=(const SDL_WindowFlags addWindowFlags) {
            windowFlags |= addWindowFlags;
            return *this;
        }
    };
}

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

namespace {
    namespace internal {
        /** Prevents the user from altering and potentially breaking the app's state; static in SDL_AppInit. */
        struct AppState {
            SDL_Window *window = nullptr;
            SDL_Renderer *renderer = nullptr;
            bool quit = false;
#ifdef OMNI_SCENE
            Omni::Scene *currentScene = nullptr;
            Omni::Scene *nextScene = nullptr;
#endif
        };

        SDL_Window *window = nullptr;
        SDL_Renderer *renderer = nullptr;
        float deltaTime = 0.0f;
        Uint32 fps = 0;
    }
}

/* ==================== OMNI DEFINITIONS ==================== */

inline SDL_Window *Omni::Window() {
    return internal::window;
}

inline SDL_Renderer *Omni::Renderer() {
    return internal::renderer;
}

inline float Omni::DeltaTime() {
    return internal::deltaTime;
}

inline Uint32 Omni::FPS() {
    return internal::fps;
}

/* ==================== SDL3 CALLBACK SETUP ==================== */

/* Runs once at startup. */
inline SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[]) {
    // Get the user's window configuration
    const WindowProperties properties = InitWindow(argc, argv);

    // Set the app metadata using the user's properties
    SDL_SetAppMetadata(properties.appName, properties.appVersion, properties.appIdentifier);

    // Set the initialization flags, VIDEO and GAMEPAD are always enabled
    if (!SDL_Init(properties.initFlags | SDL_INIT_VIDEO | SDL_INIT_GAMEPAD)) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't initialize SDL: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    // Create the app state to hide from user
    static internal::AppState appState;
    *appstate = &appState;

    // Create a window with the provided title, size, and flags; also create a renderer
    if (!SDL_CreateWindowAndRenderer(properties.title, properties.width, properties.height, properties.windowFlags,
        &appState.window, &appState.renderer)) {
        SDL_LogError(SDL_LOG_CATEGORY_VIDEO, "Couldn't create window/renderer: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    // Provide the window and renderer to allow correct functionality for Omni::Window() and Omni::Renderer().
    internal::window = appState.window;
    internal::renderer = appState.renderer;

    // Use screen viewport as default
    SDL_SetRenderLogicalPresentation(appState.renderer, 0, 0, SDL_LOGICAL_PRESENTATION_DISABLED);

    // After SDL3 and the window are prepared, call the user's Init() function
#ifdef OMNI_SCENE
    Omni::Scene* &currentScene = appState.currentScene;
    Omni::Scene* &nextScene = appState.nextScene;

    currentScene = Init(argc, argv);
    if (currentScene)
        currentScene->init();
    nextScene = currentScene;
    return currentScene ? SDL_APP_CONTINUE : SDL_APP_FAILURE;
#else
    return Init(argc, argv) ? SDL_APP_CONTINUE : SDL_APP_FAILURE;
#endif
}

/* This function runs when a new event (mouse input, keypresses, etc.) occurs. */
inline SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event) {
    if (event->type == SDL_EVENT_QUIT) {
        ((internal::AppState*) appstate)->quit = true;
        return SDL_APP_CONTINUE;
    }

    // TODO: Listen for and store inputs

    return SDL_APP_CONTINUE;
}

/* This function runs once per frame, and is the heart of the program. */
inline SDL_AppResult SDL_AppIterate(void *appstate) {
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

    internal::AppState &appState = *(internal::AppState*) appstate;

    // Clear the window to black (automatic for the user)
    SDL_SetRenderDrawColor(appState.renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(appState.renderer);

#ifdef OMNI_SCENE
    Omni::Scene* &nextScene = appState.nextScene;
    if (!nextScene) return SDL_APP_SUCCESS;
    Omni::Scene* &currentScene = appState.currentScene;

    // Changing Scenes: Dispose and free current scene, initialize the next scene and set it as current
    if (currentScene != nextScene) {
        if (currentScene->dispose())
            delete currentScene;
        nextScene->init();
        currentScene = nextScene;
    }

    // Called here to ensure the current scene and next scene always match to free memory from both properly
    // This also allows init() to be called before dispose() outside the game loop
    if (appState.quit) return SDL_APP_SUCCESS;

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

    return SDL_APP_CONTINUE;
}

/* This function runs once at shutdown; SDL will clean up the window/renderer for us. */
inline void SDL_AppQuit(void *appstate, SDL_AppResult result) {
#ifdef OMNI_SCENE
    // Dispose remaining scene (if available)
    Omni::Scene* &currentScene = ((internal::AppState*) appstate)->currentScene;
    if (currentScene && currentScene->dispose()) {
        delete currentScene;
        currentScene = nullptr;
        ((internal::AppState*) appstate)->nextScene = nullptr;
    }
#endif
    Dispose();
}
