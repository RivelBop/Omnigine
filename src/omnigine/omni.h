#pragma once

#include <SDL3/SDL.h>

namespace Omni {
    /* ==================== WINDOW & RENDERER ==================== */

    /** Access to the window pointer. */
    [[nodiscard]] SDL_Window *Window();
    /** Access to the renderer pointer. */
    [[nodiscard]] SDL_Renderer *Renderer();

    /* ==================== VIEWPORT ==================== */

    using Viewport = SDL_RendererLogicalPresentation;
    inline constexpr Viewport SCREEN_VIEWPORT        = SDL_LOGICAL_PRESENTATION_DISABLED;
    inline constexpr Viewport STRETCH_VIEWPORT       = SDL_LOGICAL_PRESENTATION_STRETCH;
    inline constexpr Viewport FIT_VIEWPORT           = SDL_LOGICAL_PRESENTATION_LETTERBOX;
    inline constexpr Viewport FILL_VIEWPORT          = SDL_LOGICAL_PRESENTATION_OVERSCAN;
    inline constexpr Viewport PIXEL_PERFECT_VIEWPORT = SDL_LOGICAL_PRESENTATION_INTEGER_SCALE;

    /** Calls SDL_SetRenderLogicalPresentation() for the global Renderer(). */
    inline bool SetViewport(const int width, const int height, const Viewport viewport) {
        return SDL_SetRenderLogicalPresentation(Renderer(), width, height, viewport);
    }

    /* ==================== CAMERA ==================== */

    struct Camera {
        float x = 0.0f;
        float y = 0.0f;
        float zoom = 1.0f;
    };

    /**
     * Projects the renderer to a camera, copies the camera internally so future camera updates must re-call this function.
     * Pass a nullptr to reset the renderer back to default values (x:0, y:0, scale:1).
     */
    void RenderToCamera(const Camera *camera);

    bool RenderPoint(float x, float y);

    bool RenderPoints(const SDL_FPoint *points, int count);

    bool RenderLine(float x1, float y1, float x2, float y2);

    bool RenderLines(const SDL_FPoint *points, int count);

    bool RenderRect(const SDL_FRect &rect);

    bool RenderRects(const SDL_FRect *rects, int count);

    bool RenderFillRect(const SDL_FRect &rect);

    bool RenderFillRects(const SDL_FRect *rects, int count);

    bool RenderTexture(SDL_Texture &texture, const SDL_FRect &srcrect, const SDL_FRect &dstrect);

    bool RenderTextureRotated(SDL_Texture &texture, const SDL_FRect &srcrect, const SDL_FRect &dstrect, double angle, const SDL_FPoint &center, SDL_FlipMode flip);

    bool RenderTextureAffine(SDL_Texture &texture, const SDL_FRect &srcrect, const SDL_FPoint &origin, const SDL_FPoint &right, const SDL_FPoint &down);

    bool RenderTextureTiled(SDL_Texture &texture, const SDL_FRect &srcrect, float scale, const SDL_FRect &dstrect);

    bool RenderTexture9Grid(SDL_Texture &texture, const SDL_FRect &srcrect, float left_width, float right_width, float top_height, float bottom_height, float scale, const SDL_FRect &dstrect);

    bool RenderTexture9GridTiled(SDL_Texture &texture, const SDL_FRect &srcrect, float left_width, float right_width, float top_height, float bottom_height, float scale, const SDL_FRect &dstrect, float tileScale);

    bool RenderGeometry(SDL_Texture &texture, const SDL_Vertex *vertices, int num_vertices, const int *indices, int num_indices);

    bool RenderGeometryRaw(SDL_Texture &texture, const float *xy, int xy_stride, const SDL_FColor *color, int color_stride, const float *uv, int uv_stride, int num_vertices, const void *indices, int num_indices, int size_indices);

    bool RenderDebugText(float x, float y, const char *str);

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
}
