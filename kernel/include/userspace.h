#ifndef KIVOS64_USERSPACE_H
#define KIVOS64_USERSPACE_H

#include "intdef.h"
#include "audio.h"
#include "controller.h"
#include "graphics.h"
#include "system.h"

void audio_play_square_wave_user(float frequency, float duration, float volume)
{
    audio_play_square_wave(frequency, duration, volume);
}

controller_buttons_t controller_poll_and_get_buttons_held_user(void)
{
    return controller_poll_and_get_buttons_held();
}

void graphics_fill_user(surface_t* surface, uint32_t color)
{
    asm volatile("move $t4, %0" : : "r" ((uint32_t) surface));
    asm volatile("move $t5, %0" : : "r" (color));
    asm volatile("li $v0, 3");
    asm volatile("syscall");
}

void graphics_draw_surface_alpha_user(surface_t* dst, int x, int y, surface_t* src)
{
    uint32_t dst_addr = ADDR_TO_KSEG0((uint32_t) dst);
    // Convert src (user sprite) to kernel segment.
    uint32_t src_addr = ADDR_TO_KSEG0((uint32_t) src);
    asm volatile("move $t4, %0" : : "r" (dst_addr));
    asm volatile("move $t5, %0" : : "r" (x));
    asm volatile("move $t6, %0" : : "r" (y));
    asm volatile("move $t7, %0" : : "r" (src_addr));
    asm volatile("li $v0, 4");
    asm volatile("syscall");
}

void display_init_user(int width, int height, filter_t filter)
{
    asm volatile("move $t4, %0" : : "r" (width));
    asm volatile("move $t5, %0" : : "r" (height));
    asm volatile("move $t6, %0" : : "r" (filter));
    asm volatile("li $v0, 5");
    asm volatile("syscall");
}

surface_t* display_get_user(void)
{
    uint32_t retval;

    asm volatile("li $v0, 6");
    asm volatile("syscall");

    asm volatile("move %0, $t8" : "=r" (retval));
    
    return (surface_t*) retval;
}

void display_show_user(surface_t* surface)
{
    asm volatile("move $t4, %0" : : "r" ((uint32_t) surface));
    asm volatile("li $v0, 7");
    asm volatile("syscall");
}

#endif
