#include "intdef.h"
#include "cop0.h"
#include "ai.h"
#include "mi.h"
#include "si.h"
#include "vi.h"
#include "system.h"
#include "interrupt.h"
#include "audio.h"
#include "controller.h"
#include "graphics.h"

/** @brief Number of nested disable interrupt calls
 *
 * This represents the number of disable interrupt calls made on the system.
 * If this is set to 0, interrupts are enabled. A number higher than 0 represents
 * that many disable calls that were nested, and consequently the number of interrupt
 * enable calls that need to be made to re-enable interrupts. A negative number means
 * that the interrupt system hasn't been initialized yet.
 */
int __interrupt_depth = -1;

int __interrupt_sr = 0;

/**
 * @brief Initialize the interrupt controller.
 */
bool interrupt_init(void)
{
    // Make sure that we aren't initializing interrupts when they are already enabled.
    if (__interrupt_depth < 0)
    {
        // Clear and mask all interrupts on the system so we start with a clean slate.
        MI_regs->mask = (MI_MASK_CLR_SP | MI_MASK_CLR_SI | MI_MASK_CLR_AI | MI_MASK_CLR_VI | MI_MASK_CLR_PI | MI_MASK_CLR_DP);

        // Set that we are enabled.
        __interrupt_depth = 0;

        // Enable interrupts.
        uint32_t sr = C0_STATUS();
        C0_WRITE_STATUS(sr | C0_STATUS_IE | C0_INTERRUPT_RCP | C0_INTERRUPT_RESET);
        return true;
    }

    return false;
}

void interrupt_disable(void)
{
    // Don't do anything if we haven't initialized.
    if (__interrupt_depth < 0)
    {
        return;
    }

    if (__interrupt_depth == 0)
    {
        // We must disable the interrupts now.
        uint32_t sr = C0_STATUS();
        C0_WRITE_STATUS(sr & ~C0_STATUS_IE);

        // Save the original SR value away, so that we know if
        // interrupts were enabled and whether to restore them.
        __interrupt_sr = sr;
    }

    // Ensure that we remember nesting levels.
    __interrupt_depth++;
}

void interrupt_enable(void)
{
    // Don't do anything if we aren't initialized.
    if (__interrupt_depth < 0)
    {
        return;
    }

    // Check that we're not calling enable_interrupts() more times than expected.
    assert(__interrupt_depth > 0, "interrupt_enable: Unbalanced call.");

    // Decrement the nesting level now that we are enabling interrupts.
    __interrupt_depth--;

    if (__interrupt_depth == 0)
    {
        // Restore the interrupt state that was active when interrupts got disabled.
        C0_WRITE_STATUS(C0_STATUS() | (__interrupt_sr & C0_STATUS_IE));
    }
}

void interrupt_set_AI(bool active)
{
    MI_regs->mask = (active) ? MI_MASK_SET_AI : MI_MASK_CLR_AI;
}

void interrupt_set_SI(bool active)
{
    MI_regs->mask = (active) ? MI_MASK_SET_SI : MI_MASK_CLR_SI;
}

void interrupt_set_VI(bool active, uint32_t line)
{
    if (active)
    {
        MI_regs->mask = MI_MASK_SET_VI;
        VI_regs->v_interrupt = line;
    }
    else
    {
        MI_regs->mask = MI_MASK_CLR_VI;
    }
}

void interrupt_reset_mode(void)
{
    // Disable FPU, reset to kernel mode, disable exception flag and disable global exceptions.
    uint32_t sr = C0_STATUS() & ~(C0_STATUS_CU1 | C0_STATUS_KSU | C0_STATUS_EXL | C0_STATUS_IE);
    C0_WRITE_STATUS(sr);
}

// Forward declare callbacks for interrupts.

// Sends next audio buffer to AI DMA to be played.
void __audio_callback(void);
// Swaps frame buffers.
void __display_callback(void);
// Issues SI DMA to read controller state.
void __joypad_callback(void);
// Updates internal controller state when SI DMA finishes.
void __controller_callback(void);

void interrupt_handler(void)
{
    // Get MI interrupt status to handle.
    uint32_t status = MI_regs->interrupt & MI_regs->mask;

    if (status & MI_INTERRUPT_AI)
    {
        // Clear interrupt.
        AI_regs->status = 0;
        __audio_callback();
    }
    if (status & MI_INTERRUPT_SI)
    {
        // Clear interrupt.
        SI_regs->status = 0;
        __controller_callback();
    }
    if (status & MI_INTERRUPT_VI)
    {
        // Clear interrupt.
    	VI_regs->v_current = 0;
        __display_callback();
        __joypad_callback();
    }
}

void exception_reset_mode(void)
{
    // Enable FPU and reentrant exception to be able to service interrupts while doing syscalls.
    uint32_t sr = C0_STATUS() | C0_STATUS_CU1 | C0_STATUS_IE;
    C0_WRITE_STATUS(sr);
}

// k0 is designated as the register holding the syscall code.
#define GET_K0() ({ \
    uint32_t value; \
    asm volatile ("move %0, $k0": "=r" (value)); \
    value; \
})
// t4-7 are designated as registers holding arguments passed by the user to syscall-calling functions.
#define GET_SYSCALL_ARG1() ({ \
    uint32_t value; \
    asm volatile ("move %0, $t4": "=r" (value)); \
    value; \
})
#define GET_SYSCALL_ARG2() ({ \
    uint32_t value; \
    asm volatile ("move %0, $t5": "=r" (value)); \
    value; \
})
#define GET_SYSCALL_ARG3() ({ \
    uint32_t value; \
    asm volatile ("move %0, $t6": "=r" (value)); \
    value; \
})
#define GET_SYSCALL_ARG4() ({ \
    uint32_t value; \
    asm volatile ("move %0, $t7": "=r" (value)); \
    value; \
})
#define SET_SYSCALL_RETVAL(value) ({ \
    asm volatile ("move $t8, %0":: "r" (value)); \
})

void exception_handler(void)
{
    uint32_t cause = C0_CAUSE() & C0_CAUSE_EXC;

    if (cause & C0_CAUSE_EXC_SYSCALL)
    {
        uint32_t syscode = GET_K0();
        if (syscode == SYSCALL_AUDIO_PLAY)
        {
            uint32_t frequency_encoded = GET_SYSCALL_ARG1();
            uint32_t duration_encoded = GET_SYSCALL_ARG2();
            uint32_t volume_encoded = GET_SYSCALL_ARG3();
            
            float frequency = (float) frequency_encoded;
            float duration = ((float) duration_encoded) / 100.0f;
            float volume = ((float) volume_encoded) / 100.0f;

            audio_play_square_wave(frequency, duration, volume);
        }
        else if (syscode == SYSCALL_CONTROLLER_POLL)
        {
            uint32_t retval = controller_poll_and_get_buttons_held().raw;
            SET_SYSCALL_RETVAL(retval);
        }
        else if (syscode == SYSCALL_GRAPHICS_FILL)
        {
            surface_t* surface = (surface_t*) GET_SYSCALL_ARG1();
            uint32_t color = GET_SYSCALL_ARG2();
            graphics_fill(surface, color);
        }
        else if (syscode == SYSCALL_GRAPHICS_DRAW_SPRITE)
        {
            surface_t* dst = (surface_t*) GET_SYSCALL_ARG1();
            int x = GET_SYSCALL_ARG2();
            int y = GET_SYSCALL_ARG3();
            surface_t* src = (surface_t*) GET_SYSCALL_ARG4();
            graphics_draw_surface_alpha(dst, x, y, src);
        }
        else if (syscode == SYSCALL_DISPLAY_INIT)
        {
            int width = GET_SYSCALL_ARG1();
            int height = GET_SYSCALL_ARG2();
            filter_t filter = GET_SYSCALL_ARG3();
            display_init(width, height, filter);
        }
        else if (syscode == SYSCALL_DISPLAY_GET)
        {
            uint32_t retval = (uint32_t) display_get();
            SET_SYSCALL_RETVAL(retval);
        }
        else if (syscode == SYSCALL_DISPLAY_SHOW)
        {
            surface_t* display = (surface_t*) GET_SYSCALL_ARG1();
            display_show(display);
        }
        else
        {
            println_u32("Unknown syscode: ", syscode);
            abort();
        }
    }
}
