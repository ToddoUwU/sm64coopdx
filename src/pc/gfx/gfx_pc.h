#ifndef GFX_PC_H
#define GFX_PC_H

#include "types.h"
#include "pc/gfx/gfx.h"

enum ShaderFlag {
    SHADER_FLAG_HUE,
    SHADER_FLAG_SATURATION,
    SHADER_FLAG_BRIGHTNESS,
    SHADER_FLAG_CONTRAST,
    SHADER_FLAG_EXPOSURE,
    SHADER_FLAG_DITHERING,
    SHADER_FLAG_POSTERIZATION,
    SHADER_FLAG_SCANLINES,
    SHADER_FLAG_MAX
};

struct GfxRenderingAPI;
struct GfxWindowManagerAPI;

extern Vec3f gLightingDir;
extern Color gLightingColor[2];
extern Color gVertexColor;
extern Color gFogColor;
extern f32 gFogIntensity;

extern bool gFullbright;

extern int gShaderFlags[SHADER_FLAG_MAX];
extern f32 gDefaultShaderFlagValues[SHADER_FLAG_MAX];
extern f32 gShaderFlagValues[SHADER_FLAG_MAX];
extern bool gShaderFlagsEnabled;

#ifdef __cplusplus
extern "C" {
#endif

void gfx_init(struct GfxWindowManagerAPI *wapi, struct GfxRenderingAPI *rapi, const char *window_title);
struct GfxRenderingAPI *gfx_get_current_rendering_api(void);
void gfx_start_frame(void);

// Split screen: rectangle (top-left origin, pixels) of view `index` out of `count` in a window of winW x winH.
// 2 views are stacked top and bottom; 3 or 4 views share a 2x2 grid.
void gfx_split_screen_view_rect(uint8_t index, uint8_t count, uint32_t winW, uint32_t winH,
                                uint32_t *x, uint32_t *y, uint32_t *w, uint32_t *h);
// Makes the current dimensions those of view `index` of `count` (count 1 = whole window). The game calls it
// while building a view's display list (with gSPView) so aspect-dependent code matches; G_VIEW_EXT calls it
// again when the display list is drawn.
void gfx_set_view(uint8_t index, uint8_t count);
void gfx_run(Gfx *commands);
void gfx_end_frame_render(void);
void gfx_display_frame(void);
void gfx_end_frame(void);
void gfx_shutdown(void);
void gfx_pc_precomp_shader(uint32_t rgb1, uint32_t alpha1, uint32_t rgb2, uint32_t alpha2, uint32_t flags);

#ifdef __cplusplus
}
#endif

#endif
