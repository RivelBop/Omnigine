/*
 * This file must only be included in exactly ONE .cpp file!
 */

#pragma once

/* ==================== IMPORTS ==================== */

#define MINIAUDIO_IMPLEMENTATION
#include <miniaudio.h>

/** Use callbacks instead of main(). */
#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include <vector>

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
    const char *appName{ nullptr };
    /** App version in metadata, can be null. */
    const char *appVersion{ nullptr };
    /** App ID in metadata, can be null. */
    const char *appIdentifier{ nullptr };

    /* ========== INIT FLAGS ========== */

    /** The flags to pass into SDL_Init(), (VIDEO | GAMEPAD) are enabled automatically. */
    SDL_InitFlags initFlags{ 0 };

    /* ========== WINDOW ========== */

    /** The window title, can be null. */
    const char *title{ nullptr };
    /** The window width. */
    int width{ 640 };
    /** The window height. */
    int height{ 480 };
    /** The window flags to pass into SDL_CreateWindowAndRenderer(), no flags by default. */
    SDL_WindowFlags windowFlags{ 0 };

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
    SDL_Window *window{ nullptr };
    SDL_Renderer *renderer{ nullptr };
    ma_engine *soundEngine{ nullptr };

    bool quit{ false };
    bool keysPressed[SDL_SCANCODE_COUNT]{};
    bool keysJustPressed[SDL_SCANCODE_COUNT]{};
    bool keyPressed{ false };

#ifdef OMNI_SCENE
    Omni::Scene *currentScene{ nullptr };
    Omni::Scene *nextScene{ nullptr };
#endif
};

/* ========== SDL_AppInit ========== */

SDL_Window *window{ nullptr };
SDL_Renderer *renderer{ nullptr };
ma_engine *soundEngine{ nullptr };

/* ========== SDL_AppEvent ========== */

const bool *keysPressed{ nullptr };
const bool *keysJustPressed{ nullptr };

/* ========== SDL_AppIterate ========== */

float deltaTime{ 0.0f };
Uint32 fps{ 0 };

/* ========== Misc. ========== */

Omni::Camera camera;
/** Efficient heap allocation when calling point-related camera-based rendering. */
std::vector<SDL_FPoint> pointBuffer;
/** Efficient heap allocation when calling rect-related camera-based rendering. */
std::vector<SDL_FRect> rectBuffer;
/** Efficient heap allocation when calling geometry or vertex related camera-based rendering. */
std::vector<SDL_Vertex> vertexBuffer;
/** Efficient heap allocation when calling raw geometry camera-based rendering. */
std::vector<float> floatBuffer;
} // namespace internal
} // namespace

/* ==================== OMNI DEFINITIONS ==================== */

/* ========== ENGINE COMPONENTS ========== */

inline SDL_Window *Omni::Window()
{
    return internal::window;
}

inline SDL_Renderer *Omni::Renderer()
{
    return internal::renderer;
}

inline ma_engine *Omni::SoundEngine()
{
    return internal::soundEngine;
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
    // This is how SDL_RenderPoint() is implemented
    SDL_FPoint fpoint{ x - internal::camera.x, y - internal::camera.y };
    return SDL_RenderPoints(internal::renderer, &fpoint, 1);
}

inline bool Omni::RenderPoints(const SDL_FPoint *points, int count)
{
    // Ensure points are available and the camera is set to a non-default position
    if (points && count > 0 && (internal::camera.x != 0.0f || internal::camera.y != 0.0f)) {
        // Camera data
        float x{ internal::camera.x };
        float y{ internal::camera.y };

        // Ensure enough buffer capacity and set the necessary size
        internal::pointBuffer.resize(count);

        // Create a camera-projected variant of each point
        // This should be efficient as drawing each point with SDL_RenderPoint() calls SDL_RenderPoints() anyway
        for (int i{ 0 }; i < count; i++)
            internal::pointBuffer[i] = { points[i].x - x, points[i].y - y };

        // Render the camera-projected points
        return SDL_RenderPoints(internal::renderer, internal::pointBuffer.data(), count);
    }

    // Handles errors if no points provided and rendering with default camera position (0,0)
    return SDL_RenderPoints(internal::renderer, points, count);
}

inline bool Omni::RenderLine(float x1, float y1, float x2, float y2)
{
    float x{ internal::camera.x };
    float y{ internal::camera.y };
    return SDL_RenderLine(internal::renderer, x1 - x, y1 - y, x2 - x, y2 - y);
}

inline bool Omni::RenderLines(const SDL_FPoint *points, int count)
{
    // Ensure points are available and the camera is set to a non-default position
    if (points && count > 1 && (internal::camera.x != 0.0f || internal::camera.y != 0.0f)) {
        // Camera data
        float x{ internal::camera.x };
        float y{ internal::camera.y };

        // Ensure enough buffer capacity and set the necessary size
        internal::pointBuffer.resize(count);

        // Create a camera-projected variant of each point
        // This should be efficient as drawing each line with SDL_RenderLine() calls SDL_RenderLines() anyway
        for (int i{ 0 }; i < count; i++)
            internal::pointBuffer[i] = { points[i].x - x, points[i].y - y };

        // Render the camera-projected lines
        return SDL_RenderLines(internal::renderer, internal::pointBuffer.data(), count);
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
    if (rects && count > 0 && (internal::camera.x != 0.0f || internal::camera.y != 0.0f)) {
        // Camera data
        float x{ internal::camera.x };
        float y{ internal::camera.y };
        SDL_FRect camRect;

        // SDL_RenderRects() also iterates and calls SDL_RenderRect() for each rect.
        // For efficiency, we do the same but reuse camRect to apply the camera position.
        for (int i{ 0 }; i < count; i++) {
            const SDL_FRect &rect = rects[i];
            camRect.x = rect.x - x;
            camRect.y = rect.y - y;
            camRect.w = rect.w;
            camRect.h = rect.h;

            if (!SDL_RenderRect(internal::renderer, &camRect))
                return false;
        }

        return true;
    }

    // Handles errors if no rects provided and rendering with default camera position (0,0)
    return SDL_RenderRects(internal::renderer, rects, count);
}

inline bool Omni::RenderFillRect(SDL_FRect rect)
{
    rect.x -= internal::camera.x;
    rect.y -= internal::camera.y;
    return SDL_RenderFillRect(internal::renderer, &rect);
}

inline bool Omni::RenderFillRects(const SDL_FRect *rects, int count)
{
    // Ensure rects are available and the camera is set to a non-default position
    if (rects && count > 0 && (internal::camera.x != 0.0f || internal::camera.y != 0.0f)) {
        // Camera data
        float x{ internal::camera.x };
        float y{ internal::camera.y };

        // Ensure enough buffer capacity and set the necessary size
        internal::rectBuffer.resize(count);

        // Create a camera-projected variant of each rect
        // This should be efficient as drawing each rect with SDL_RenderFillRect() calls SDL_RenderFillRects() anyway
        for (int i{ 0 }; i < count; i++)
            internal::rectBuffer[i] = { rects[i].x - x, rects[i].y - y, rects[i].w, rects[i].h };

        // Render the camera-projected rects
        return SDL_RenderFillRects(internal::renderer, internal::rectBuffer.data(), count);
    }

    // Handles errors if no rects provided and rendering with default camera position (0,0)
    return SDL_RenderFillRects(internal::renderer, rects, count);
}

inline bool Omni::RenderTexture(SDL_Texture *texture, const SDL_FRect *srcrect, SDL_FRect dstrect)
{
    dstrect.x -= internal::camera.x;
    dstrect.y -= internal::camera.y;
    return SDL_RenderTexture(internal::renderer, texture, srcrect, &dstrect);
}

inline bool Omni::RenderTextureRotated(SDL_Texture *texture, const SDL_FRect *srcrect, SDL_FRect dstrect, double angle, const SDL_FPoint *center, SDL_FlipMode flip)
{
    dstrect.x -= internal::camera.x;
    dstrect.y -= internal::camera.y;
    return SDL_RenderTextureRotated(internal::renderer, texture, srcrect, &dstrect, angle, center, flip);
}

inline bool Omni::RenderTextureAffine(SDL_Texture *texture, const SDL_FRect *srcrect, SDL_FPoint origin, SDL_FPoint right, SDL_FPoint down)
{
    // Shift the local copies by the camera position
    origin.x -= internal::camera.x;
    origin.y -= internal::camera.y;

    right.x -= internal::camera.x;
    right.y -= internal::camera.y;

    down.x -= internal::camera.x;
    down.y -= internal::camera.y;

    return SDL_RenderTextureAffine(internal::renderer, texture, srcrect, &origin, &right, &down);
}

inline bool Omni::RenderTextureTiled(SDL_Texture *texture, const SDL_FRect *srcrect, float scale, SDL_FRect dstrect)
{
    dstrect.x -= internal::camera.x;
    dstrect.y -= internal::camera.y;
    return SDL_RenderTextureTiled(internal::renderer, texture, srcrect, scale, &dstrect);
}

inline bool Omni::RenderTexture9Grid(SDL_Texture *texture, const SDL_FRect *srcrect, float leftWidth, float rightWidth, float topHeight, float bottomHeight, float scale, SDL_FRect dstrect)
{
    dstrect.x -= internal::camera.x;
    dstrect.y -= internal::camera.y;
    return SDL_RenderTexture9Grid(internal::renderer, texture, srcrect, leftWidth, rightWidth, topHeight, bottomHeight, scale, &dstrect);
}

inline bool Omni::RenderTexture9GridTiled(SDL_Texture *texture, const SDL_FRect *srcrect, float leftWidth, float rightWidth, float topHeight, float bottomHeight, float scale, SDL_FRect dstrect, float tileScale)
{
    dstrect.x -= internal::camera.x;
    dstrect.y -= internal::camera.y;
    return SDL_RenderTexture9GridTiled(internal::renderer, texture, srcrect, leftWidth, rightWidth, topHeight, bottomHeight, scale, &dstrect, tileScale);
}

inline bool Omni::RenderGeometry(SDL_Texture *texture, const SDL_Vertex *vertices, int numVertices, const int *indices, int numIndices)
{
    // Ensure vertices provided and the camera is set to a non-default position
    if (vertices && numVertices > 0 && (internal::camera.x != 0.0f || internal::camera.y != 0.0f)) {
        // Camera data
        float x{ internal::camera.x };
        float y{ internal::camera.y };

        // Ensure enough buffer capacity and set the necessary size
        internal::vertexBuffer.resize(numVertices);

        // Create a camera-projected variant of each vertex
        for (int i{ 0 }; i < numVertices; i++) {
            SDL_Vertex vertex{ vertices[i] };
            vertex.position.x -= x;
            vertex.position.y -= y;
            internal::vertexBuffer[i] = vertex;
        }

        // Render the camera-projected vertices
        return SDL_RenderGeometry(internal::renderer, texture, internal::vertexBuffer.data(), numVertices, indices, numIndices);
    }

    // Handles errors if no vertices and rendering with default camera position (0,0)
    return SDL_RenderGeometry(internal::renderer, texture, vertices, numVertices, indices, numIndices);
}

inline bool Omni::RenderGeometryRaw(SDL_Texture *texture, const float *xy, int xyStride, const SDL_FColor *color, int colorStride, const float *uv, int uvStride, int numVertices, const void *indices, int numIndices, int sizeIndices)
{
    // Perform some of the required checks SDL_RenderGeometryRaw() does and ensure the camera is at a non-default position
    if (xy && color && (!texture || uv) && numVertices > 2 && (internal::camera.x != 0.0f || internal::camera.y != 0.0f)) {
        // Camera data
        float x{ internal::camera.x };
        float y{ internal::camera.y };

        // Ensure enough buffer capacity and set the necessary size
        // xy is represented as 2 floats (x and y) side-by-side so 2x the # of vertices is necessary
        internal::floatBuffer.resize(numVertices * 2);

        // Create a camera-projected variant of each vertex
        for (int i{ 0 }; i < numVertices; i++) {
            // Represent xy pointer as char for byte-size pointer arithmetic (xyStride is in bytes)
            // Start at the first xy, move up by the xyStride and the index
            const float *pos{ reinterpret_cast<const float *>(reinterpret_cast<const char *>(xy) + xyStride * i) };

            // The float buffer's index stores side-by-side floats representing x and y therefore i * 2 is x
            int index{ i * 2 };
            internal::floatBuffer[index] = pos[0] - x;
            internal::floatBuffer[index + 1] = pos[1] - y;
        }

        // Render the camera-projected vertices
        return SDL_RenderGeometryRaw(internal::renderer, texture, internal::floatBuffer.data(), sizeof(float) * 2, color, colorStride, uv, uvStride, numVertices, indices, numIndices, sizeIndices);
    }

    // Handles missing data, errors, and rendering with default camera position (0,0)
    return SDL_RenderGeometryRaw(internal::renderer, texture, xy, xyStride, color, colorStride, uv, uvStride, numVertices, indices, numIndices, sizeIndices);
}

inline bool Omni::RenderDebugText(float x, float y, const char *str)
{
    return SDL_RenderDebugText(internal::renderer, x - internal::camera.x, y - internal::camera.y, str);
}

inline bool Omni::RenderDebugTextFormat(float x, float y, const char *fmt, ...)
{
    // The following is taken straight from SDL_RenderDebugTextFormat()
    va_list ap;
    va_start(ap, fmt);

    x -= internal::camera.x;
    y -= internal::camera.y;

    if (SDL_strcmp(fmt, "%s") == 0) {
        const char *str{ va_arg(ap, const char *) };
        va_end(ap);
        return SDL_RenderDebugText(internal::renderer, x, y, str);
    }

    char *str{ nullptr };
    const int rc{ SDL_vasprintf(&str, fmt, ap) };
    va_end(ap);

    if (rc == -1)
        return false;

    const bool retval{ SDL_RenderDebugText(internal::renderer, x, y, str) };
    SDL_free(str);
    return retval;
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
    const WindowProperties properties{ InitWindow(argc, argv) };

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
    internal::soundEngine = appState.soundEngine;

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
    internal::AppState &appState = *static_cast<internal::AppState *>(appstate);

    if (event->type == SDL_EVENT_QUIT) {
        appState.quit = true;
        return SDL_APP_CONTINUE;
    }

    if (event->type == SDL_EVENT_KEY_DOWN) {
        SDL_Scancode key{ event->key.scancode };
        if (!appState.keysPressed[key]) {
            appState.keysPressed[key] = true;
            appState.keysJustPressed[key] = true;
            appState.keyPressed = true;
        }
    } else if (event->type == SDL_EVENT_KEY_UP) {
        SDL_Scancode key{ event->key.scancode };
        appState.keysPressed[key] = false;
        appState.keysJustPressed[key] = false;
    }

    return SDL_APP_CONTINUE;
}

/* This function runs once per frame, and is the heart of the program. */
inline SDL_AppResult SDL_AppIterate(void *appstate)
{
    static Uint64 startDt{ SDL_GetTicksNS() };
    static Uint64 startFps{ startDt };
    static Uint32 frames{ 0 };
    const Uint64 end{ SDL_GetTicksNS() };

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

    internal::AppState &appState = *static_cast<internal::AppState *>(appstate);

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
        memset(appState.keysJustPressed, 0, SDL_SCANCODE_COUNT * sizeof(bool));
        appState.keyPressed = false;
    }

    return SDL_APP_CONTINUE;
}

/* This function runs once at shutdown; SDL will clean up the window/renderer for us. */
inline void SDL_AppQuit(void *appstate, SDL_AppResult result)
{
    internal::AppState &appState = *static_cast<internal::AppState *>(appstate);
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
        internal::soundEngine = nullptr;
    }
}
