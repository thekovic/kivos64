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

}
