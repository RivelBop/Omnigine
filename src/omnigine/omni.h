#pragma once

#include <SDL3/SDL.h>

namespace Omni
{
/* ==================== WINDOW & RENDERER ==================== */

/** Access to the window pointer. */
[[nodiscard]] SDL_Window *Window();
/** Access to the renderer pointer. */
[[nodiscard]] SDL_Renderer *Renderer();

/** Calls SDL_SetRenderDrawColorFloat() for the global Renderer(). */
inline bool SetRenderColor(Uint8 r, Uint8 g, Uint8 b, Uint8 a)
{
    return SDL_SetRenderDrawColorFloat(Renderer(), r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f);
}

/**
 * Calls SDL_SetRenderDrawColorFloat() for the global Renderer().
 * Set each color to a value between 0-255.
 */
inline bool SetRenderColor(int r, int g, int b, int a)
{
    return SDL_SetRenderDrawColorFloat(Renderer(), r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f);
}

/**
 * Calls SDL_SetRenderDrawColorFloat() for the global Renderer().
 * Set each color to a value between 0.0f-1.0f, calculated by dividing a value between 0-255 by 255.0f.
 */
inline bool SetRenderColor(float r, float g, float b, float a)
{
    return SDL_SetRenderDrawColorFloat(Renderer(), r, g, b, a);
}

/* ==================== VIEWPORT ==================== */

using Viewport = SDL_RendererLogicalPresentation;
inline constexpr Viewport SCREEN_VIEWPORT{ SDL_LOGICAL_PRESENTATION_DISABLED };
inline constexpr Viewport STRETCH_VIEWPORT{ SDL_LOGICAL_PRESENTATION_STRETCH };
inline constexpr Viewport FIT_VIEWPORT{ SDL_LOGICAL_PRESENTATION_LETTERBOX };
inline constexpr Viewport FILL_VIEWPORT{ SDL_LOGICAL_PRESENTATION_OVERSCAN };
inline constexpr Viewport PIXEL_PERFECT_VIEWPORT{ SDL_LOGICAL_PRESENTATION_INTEGER_SCALE };

/** Calls SDL_SetRenderLogicalPresentation() for the global Renderer(). */
inline bool SetViewport(const int width, const int height, const Viewport viewport)
{
    return SDL_SetRenderLogicalPresentation(Renderer(), width, height, viewport);
}

/* ==================== CAMERA ==================== */

struct Camera
{
    float x{ 0.0f };
    float y{ 0.0f };
    float zoom{ 1.0f };
};

/**
 * Projects the renderer to a camera, copies the camera internally so future camera updates must re-call this function.
 * Pass a nullptr to reset the renderer back to default values (x:0, y:0, scale:1).
 */
void RenderToCamera(const Camera *camera);

/** Renders a point through the camera to the renderer. */
bool RenderPoint(float x, float y);

/** Renders points through the camera to the renderer. */
bool RenderPoints(const SDL_FPoint *points, int count);

/** Renders a line through the camera to the renderer. */
bool RenderLine(float x1, float y1, float x2, float y2);

/** Renders lines through the camera to the renderer. */
bool RenderLines(const SDL_FPoint *points, int count);

/** Renders a rectangle through the camera to the renderer. */
bool RenderRect(SDL_FRect rect);

/** Renders rectangles through the camera to the renderer. */
bool RenderRects(const SDL_FRect *rects, int count);

/** Renders a filled rectangle through the camera to the renderer. */
bool RenderFillRect(SDL_FRect rect);

/** Renders filled rectangles through the camera to the renderer. */
bool RenderFillRects(const SDL_FRect *rects, int count);

/** Renders a texture through the camera to the renderer, pass nullptr for srcrect to use the entire texture. */
bool RenderTexture(SDL_Texture *texture, const SDL_FRect *srcrect, SDL_FRect dstrect);

/** Renders a rotated texture (in clockwise degrees) through the camera to the renderer, pass nullptr for srcrect to use the entire texture, pass nullptr for center to use half dstrect's size. */
bool RenderTextureRotated(SDL_Texture *texture, const SDL_FRect *srcrect, SDL_FRect dstrect, double angle, const SDL_FPoint *center, SDL_FlipMode flip);

/** Renders an affine texture (for perspective) through the camera to the renderer, pass nullptr for srcrect to use the entire texture. */
bool RenderTextureAffine(SDL_Texture *texture, const SDL_FRect *srcrect, SDL_FPoint origin, SDL_FPoint right, SDL_FPoint down);

/** Renders a texture as a tile to fill dstrect through the camera to the renderer, pass nullptr for srcrect to use the entire texture. */
bool RenderTextureTiled(SDL_Texture *texture, const SDL_FRect *srcrect, float scale, SDL_FRect dstrect);

/** Renders a texture using 9-slice scaling (ideal for UI) through the camera to the renderer, pass nullptr for srcrect to use the entire texture. */
bool RenderTexture9Grid(SDL_Texture *texture, const SDL_FRect *srcrect, float leftWidth, float rightWidth, float topHeight, float bottomHeight, float scale, SDL_FRect dstrect);

/** Renders a texture using 9-slice scaling as a tile to fill dstrect (ideal for UI relying on repeating textures) through the camera to the renderer, pass nullptr for srcrect to use the entire texture. */
bool RenderTexture9GridTiled(SDL_Texture *texture, const SDL_FRect *srcrect, float leftWidth, float rightWidth, float topHeight, float bottomHeight, float scale, SDL_FRect dstrect, float tileScale);

/** Renders any geometric shape using a (optional) texture, vertices, and (optional) indices through the camera to the renderer. */
bool RenderGeometry(SDL_Texture *texture, const SDL_Vertex *vertices, int numVertices, const int *indices, int numIndices);

/** Renders any geometric shape using a (optional) texture, raw vertice data, and (optional) indices through the camera to the renderer. */
bool RenderGeometryRaw(SDL_Texture *texture, const float *xy, int xyStride, const SDL_FColor *color, int colorStride, const float *uv, int uvStride, int numVertices, const void *indices, int numIndices, int sizeIndices);

/** Renders debug text through the camera to the renderer. */
bool RenderDebugText(float x, float y, const char *str);

/** Renders debug text with C-style formatting through the camera to the renderer. */
bool RenderDebugTextFormat(float x, float y, const char *fmt, ...);

/* ==================== INPUTS ==================== */

/** True if key scancode is pressed. Uses physical key location, best for movement keys (WASD, etc.). */
[[nodiscard]] bool IsKeyPressed(SDL_Scancode key);

/** True if keycode is pressed. Uses virtual key mapping. */
[[nodiscard]] bool IsKeyPressed(SDL_Keycode key);

/** True for one frame if key scancode is pressed. Uses physical key location. */
[[nodiscard]] bool IsKeyJustPressed(SDL_Scancode key);

/** True for one frame if keycode is pressed. Uses virtual key mapping, best for specific key shortcuts (I for inventory, etc.). */
[[nodiscard]] bool IsKeyJustPressed(SDL_Keycode key);

/* ==================== GAME LOOP ==================== */

/** Returns the time it took for the previous frame to complete and start the next frame. */
[[nodiscard]] float DeltaTime();
/** Returns frames per second. */
[[nodiscard]] Uint32 FPS();
} // namespace Omni
