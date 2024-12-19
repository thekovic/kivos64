#ifndef KIVOS64_INTERRUPT_H
#define KIVOS64_INTERRUPT_H

typedef enum
{
    SYSCALL_AUDIO_PLAY = 1,
    SYSCALL_CONTROLLER_POLL,
    SYSCALL_GRAPHICS_FILL,
    SYSCALL_GRAPHICS_DRAW_SPRITE,
    SYSCALL_DISPLAY_INIT,
    SYSCALL_DISPLAY_GET,
    SYSCALL_DISPLAY_SHOW,
    SYSCALL_TEST = 42
} syscall_t;

void interrupt_disable(void);

void interrupt_enable(void);

void interrupt_set_AI(bool active);

void interrupt_set_SI(bool active);

void interrupt_set_VI(bool active, uint32_t line);

#endif
