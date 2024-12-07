#ifndef KIVOS64_GRAPHICS_H
#define KIVOS64_GRAPHICS_H

#include "intdef.h"

/** 
 * @brief Create a uint32_t representing color from R,G,B,A components in the RGBA32 range (0-255).
 */
#define RGBA32(r,g,b,a) ({ \
    (uint32_t) ((((uint8_t) r) << 24) | (((uint8_t) g) << 16) | (((uint8_t) b) << 8) | ((uint8_t) a)); \
})

/**
 * @brief This structure holds the basic information about a buffer used to hold graphics.
 */
typedef struct surface_s
{
    uint16_t width;       // Width in pixels.
    uint16_t height;      // Height in pixels.
    uint32_t* buffer;     // Buffer pointer.
} surface_t;

/**
 * @brief
 * 
 * Allocate a new surface in memory. The surface must be freed via #surface_free
 * when it is not needed anymore.
 *
 * @param[in]  width    Width in pixels.
 * @param[in]  height   Height in pixels.
 * @return              The initialized surface.
 */
surface_t surface_alloc(uint16_t width, uint16_t height);

/**
 * @brief
 * 
 * This function should be called after a surface allocated via #surface_alloc is not
 * needed anymore. 
 * 
 * @param[in]  surface   The surface to free.
 */
void surface_free(surface_t surface);

/**
 * @brief Draw a pixel to a given surface.
 *
 * @note This function does not support transparency for speed purposes.  To draw
 * a transparent pixel, use #graphics_draw_pixel_alpha.
 * 
 * @note This function does not support clipping. Specifying x/y out of surface
 * bounds will result in writing to unintended memory.
 *
 * @param[in]  surface  The surface to draw to.
 * @param[in]  x        The x coordinate of the pixel.
 * @param[in]  y        The y coordinate of the pixel.
 * @param[in]  color    The 32-bit RGBA color to draw to the screen.
 */
void graphics_draw_pixel(surface_t* surface, int x, int y, uint32_t color);

/**
 * @brief Draw an alpha-blended pixel to a given surface.
 * 
 * @note This function does not support clipping. Specifying x/y out of surface
 * bounds will result in writing to unintended memory.
 *
 * @param[in]  surface  The surface to draw to.
 * @param[in]  x        The x coordinate of the pixel.
 * @param[in]  y        The y coordinate of the pixel.
 * @param[in]  color    The 32-bit RGBA color to draw to the screen.
 */
void graphics_draw_pixel_alpha(surface_t* surface, int x, int y, uint32_t color);

/**
 * @brief 
 * 
 * @param[in]  surface  The surface to draw to.
 * @param[in]  color 
 */
void graphics_fill(surface_t* surface, uint32_t color);

#endif
