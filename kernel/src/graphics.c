#include "graphics.h"
#include "memory.h"

surface_t surface_alloc(uint16_t width, uint16_t height)
{
    return (surface_t) {
        .width = width,
        .height = height,
        .buffer = NULL
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

}
