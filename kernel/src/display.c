#include "system.h"
#include "vi.h"
#include "graphics.h"
#include "memory.h"
#include "interrupt.h"

/** @brief Maximum number of framebuffers. */
#define NUM_BUFFERS         (2)

/** @brief Width of currently active display. */
static uint32_t __width;
/** @brief Height of currently active display. */
static uint32_t __height;
/** @brief Surface structs for display framebuffers. */
static surface_t __surfaces[NUM_BUFFERS];
/** @brief Direct pointers to buffers. */
static uint32_t* __buffers[NUM_BUFFERS];
/** @brief Index of currently displayed buffer. */
static int __now_showing = -1;
/** @brief Bitmask of surfaces that are acquired to be drawn to. */
static uint32_t __acquired_mask = 0;
/** @brief Bitmask of surfaces that are pending to be displayed. */
static uint32_t __pending_mask = 0;

/** @brief Get the next buffer index (with wrap around). */
static inline int __display_next_buffer(int id)
{
    id++;
    if (id == NUM_BUFFERS)
    {
        id = 0;
    }

    return id;
}

/**
 * @brief Interrupt handler for vertical blank.
 *
 * If there is another frame to display, display the frame.
 */
void __display_callback()
{
    int next = __display_next_buffer(__now_showing);
    // Check if the next buffer is set to be displayed,
    // otherwise just leave up the current frame.
    if (__pending_mask & (1 << next))
    {
        __now_showing = next;
        __pending_mask &= ~(1 << next);
    }

    VI_regs->origin = (uint32_t) __buffers[__now_showing];
}

void display_init(int width, int height, filter_t filter)
{
    // Can't have the video interrupt happening here.
    interrupt_disable();

    // Validate display size.
    assert(width > 0, "display_init: Negative width not allowed.");
    assert(height > 0, "display_init: Negative height not allowed.");
    assert(width <= 640, "display_init: Heights > 640 don't make sense on real hardware.");
    assert(height <= 576, "display_init: Heights > 576 don't make sense on real hardware.");
    assert(width % 2 == 0, "display_init: Width must be divisible by 2 for 32-bit depth.");

    __width = width;
    __height = height;

    uint32_t control = (VI_CTRL_TYPE_32_BPP | VI_CTRL_PIXEL_ADVANCE_DEFAULT);
    if (filter == FILTER_NONE)
    {
        control |= VI_CTRL_AA_NONE;
    }
    else
    {
        control |= VI_CTRL_AA_RESAMPLE;
    }

    // Initialize buffers.
    for (int i = 0; i < NUM_BUFFERS; i++ )
    {
        __surfaces[i] = surface_alloc(__width, __height);
        __buffers[i] = __surfaces[i].buffer;
        assert(__buffers[i] != NULL, "display_init: Failed to allocate display framebuffer.");
        memset(__buffers[i], 0, __width * __height * sizeof(uint32_t));
    }

    // Set the first buffer as the displaying buffer.
    __now_showing = 0;
    __acquired_mask = 0;
    __pending_mask = 0;

    // Wait for vblank.
    while(VI_regs->v_current != VI_V_CURRENT_VBLANK ) {  }

    VI_regs->origin = (uint32_t) __buffers[0];
    VI_regs->width = __width;
    VI_regs->x_scale = VI_X_SCALE_SET(__width, 640);
    VI_regs->y_scale = VI_Y_SCALE_SET(__height, 240);
    VI_regs->control = control;

    interrupt_enable();

    // Enable VI interrupt.
    interrupt_set_VI(true, VI_V_CURRENT_VBLANK);
}

static surface_t* display_try_get(void)
{
    surface_t* display = NULL;

    interrupt_disable();

    // Calculate index of next frame buffer to draw to. We need to find
    // the first buffer which is not already acquired or pending to be displayed.
    int next = __display_next_buffer(__now_showing);
    do
    {
        if (!((__acquired_mask | __pending_mask) & (1 << next)))
        {
            display = &__surfaces[next];
            __acquired_mask |= 1 << next;
            break;
        }
        next = __display_next_buffer(next);
    } while (next != __now_showing);

    interrupt_enable();

    return display;
}

surface_t* display_get(void)
{
    surface_t* display = NULL;

    while (display == NULL)
    {
        display = display_try_get();
    }

    return display;
}

void display_show(surface_t* surface)
{
    if (surface == NULL)
    {
        return;
    }

    interrupt_disable();

    int i = surface - __surfaces;

    assert(i >= 0 && i < NUM_BUFFERS, "display_show: Display address is not valid!");
    assert(!(__pending_mask & (1 << i)), "display_show: Called on the same display twice.");
    assert(__acquired_mask & (1 << i), "display_show: Called on a display not acquired first.");

    __acquired_mask &= ~(1 << i);
    __pending_mask |= 1 << i;

    interrupt_enable();
}
