#include "graphics.h"
#include "memory.h"

surface_t surface_alloc(uint16_t width, uint16_t height)
{
    return (surface_t) {
        .width = width,
        .height = height,
        .buffer = malloc_uncached(height * width * sizeof(uint32_t))
    };
}

void surface_free(surface_t surface)
{
}

static inline uint32_t get_pixel(uint32_t* buffer, int index)
{
    return buffer[index];
}

static inline void draw_pixel(uint32_t* buffer, int index, uint32_t color)
{
    buffer[index] = color;
}

static uint32_t alphablend_pixel(uint32_t current_color, uint32_t drawn_color)
{
    uint8_t current_r = (current_color >> 24) & 0xFF;
    uint8_t current_g = (current_color >> 16) & 0xFF;
    uint8_t current_b = (current_color >>  8) & 0xFF;

    uint8_t r = (drawn_color >> 24) & 0xFF;
    uint8_t g = (drawn_color >> 16) & 0xFF;
    uint8_t b = (drawn_color >>  8) & 0xFF;
    float a = (drawn_color & 0xFF) / 255.0f;

    uint8_t new_r = (r - current_r) * a + current_r;
    uint8_t new_g = (g - current_g) * a + current_g;
    uint8_t new_b = (b - current_b) * a + current_b;

    return RGBA32(new_r, new_g, new_b, 0xFF);
}

void graphics_fill(surface_t* surface, uint32_t color)
{
    if (surface->buffer == NULL)
    {
        return;
    }

    for (int i = 0; i < surface->width * surface->height; i++)
    {
        draw_pixel(surface->buffer, i, color);
    }
}

void graphics_draw_pixel(surface_t* surface, int x, int y, uint32_t color)
{
    if (surface->buffer == NULL)
    {
        return;
    }

    draw_pixel(surface->buffer, x + (y * surface->width), color);
}

void graphics_draw_pixel_alpha(surface_t* surface, int x, int y, uint32_t color)
{
    if (surface->buffer == NULL)
    {
        return;
    }

    int index = x + (y * surface->width);

    // Defer to basic drawing if color is fully opaque.
    if ((color & 0xFF) == 0xFF)
    {
        draw_pixel(surface->buffer, index, color);
        return;
    }

    uint32_t current_color = get_pixel(surface->buffer, index);
    uint32_t new_color = alphablend_pixel(current_color, color);
    draw_pixel(surface->buffer, index, new_color);
}

typedef struct clip_area_s
{
    int x_start;
    int y_start;
    int x_end;
    int y_end;
} clip_area_t;

static clip_area_t clip_surface(surface_t* dst, int x, int y, surface_t* src)
{
    // Source surface bounds.
    int start_x = 0;
    int start_y = 0;
    int end_x = src->width;
    int end_y = src->height;

    // Clip left.
    if (x < 0)
    {
        start_x += (x * -1);
    }
    // Clip top.
    if (y < 0)
    {
        start_y += (y * -1);
    }
    // Clip right.
    if ((x + end_x) >= (int) dst->width)
    {
        end_x = dst->width - x;
    }
    // Clipbottom.
    if ((y + end_y) >= (int) dst->height)
    {
        end_y = dst->height - y;
    }

    return (clip_area_t) {.x_start = start_x, .y_start = start_y, .x_end = end_x, .y_end = end_y};
}

void graphics_draw_surface(surface_t* dst, int x, int y, surface_t* src)
{
    // Sanity checking
    if (dst->buffer == NULL)
    {
        return;
    }
    if (src->buffer == NULL)
    {
        return;
    }

    // Exit early if all drawing would go off-surface.
    if (((x + (int) src->width) <= 0) ||
        ((y + (int) src->height) <= 0) ||
        (x >= (int) dst->width) ||
        (y >= (int) dst->height))
    {
        return;
    }

    clip_area_t clip_area = clip_surface(dst, x, y, src);

    for (int src_row = clip_area.y_start; src_row < clip_area.y_end; src_row++ )
    {
        const int y_index = src_row * src->width;

        for (int src_col = clip_area.x_start; src_col < clip_area.x_end; src_col++ )
        {
            const int index = (x + src_col) + ((y + src_row) * dst->width);
            draw_pixel(dst->buffer, index, get_pixel(src->buffer, y_index + src_col));
        }
    }
}

void graphics_draw_surface_alpha(surface_t* dst, int x, int y, surface_t* src)
{
    // Sanity checking
    if (dst->buffer == NULL)
    {
        return;
    }
    if (src->buffer == NULL)
    {
        return;
    }

    // Exit early if all drawing would go off-surface.
    if (((x + (int) src->width) <= 0) ||
        ((y + (int) src->height) <= 0) ||
        (x >= (int) dst->width) ||
        (y >= (int) dst->height))
    {
        return;
    }

    clip_area_t clip_area = clip_surface(dst, x, y, src);

    for (int src_row = clip_area.y_start; src_row < clip_area.y_end; src_row++ )
    {
        const int y_index = src_row * src->width;

        for (int src_col = clip_area.x_start; src_col < clip_area.x_end; src_col++ )
        {
            const int index = (x + src_col) + ((y + src_row) * dst->width);
            const uint32_t src_color = get_pixel(src->buffer, y_index + src_col);
            // Defer to basic drawing if color is fully opaque.
            if ((src_color & 0xFF) == 0xFF)
            {
                draw_pixel(dst->buffer, index, src_color);
            }
            else
            {
                const uint32_t dst_color = get_pixel(dst->buffer, index);
                uint32_t new_color = alphablend_pixel(dst_color, src_color);
                draw_pixel(dst->buffer, index, new_color);
            }
        }
    }
}
