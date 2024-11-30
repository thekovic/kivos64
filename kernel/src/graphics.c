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

void graphics_draw_pixel(surface_t* surface, int x, int y, uint32_t color)
{
    if (surface->buffer == NULL)
    {
        return;
    }

    surface->buffer[x + (y * surface->width)] = color;
}
