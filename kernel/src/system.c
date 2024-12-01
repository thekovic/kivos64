#include "system.h"
#include "cop0.h"
#include "cop1.h"
#include "mi.h"
#include "pi.h"
#include "vi.h"
#include "interrupt.h"

int __boot_memsize;
int __boot_random_seed;
int __boot_tvtype;
int __boot_resettype;

volatile MI_registers_t* const MI_regs = (MI_registers_t*) MI_REG_BASE;
volatile PI_registers_t* const PI_regs = (PI_registers_t*) PI_REG_BASE;
volatile VI_registers_t* const VI_regs = (VI_registers_t*) VI_REG_BASE;

void cop0_status_reset(void)
{
    C0_WRITE_STATUS(C0_STATUS_CU1 | C0_STATUS_SR | C0_STATUS_FR | C0_STATUS_KX | C0_STATUS_SX | C0_STATUS_UX);
}

void cop1_init(void)
{
    // Read initialized value from COP1 status register.
    uint32_t fcr31 = C1_FCR31();
    
    // Disable all pending exceptions to avoid triggering one immediately.
    // These can survive a soft reset.
    fcr31 &= ~(C1_CAUSE_OVERFLOW | C1_CAUSE_UNDERFLOW | C1_CAUSE_NOT_IMPLEMENTED | 
               C1_CAUSE_DIV_BY_0 | C1_CAUSE_INEXACT_OP | C1_CAUSE_INVALID_OP);

    /* Set FS bit to allow flushing of denormalized floats
       The FPU inside the N64 CPU does not implement denormalized floats
       and will generate an unmaskable exception if a denormalized float
       is generated by any floating point operations. In order to prevent
       this exception, we set the FS bit in the FCR31 status register to
       instead "flush" the denormalized number. */
    fcr31 |= C1_STATUS_FS;

    // Write back updated COP1 status register.
    C1_WRITE_FCR31(fcr31);
}

void vi_init(void)
{
    VI_regs->control = (VI_CTRL_TYPE_32_BPP | VI_CTRL_AA_RESAMPLE | VI_CTRL_PIXEL_ADVANCE_DEFAULT);
    VI_regs->origin = VI_ORIGIN_SET(0);
    VI_regs->width = VI_WIDTH_SET(320);
    VI_regs->v_interrupt = VI_V_INTR_SET(2);
    VI_regs->v_current = 0;
    VI_regs->h_burst = VI_H_BURST_SET(VI_BURST_START_NTSC, VI_VSYNC_WIDTH_NTSC, VI_BURST_WIDTH_NTSC, VI_HSYNC_WIDTH_NTSC);
    VI_regs->v_total = VI_V_TOTAL_SET(525);
    VI_regs->h_total = VI_H_TOTAL_SET(0, 3093);
    VI_regs->h_total_leap = VI_H_TOTAL_LEAP_SET(3093, 3093);
    VI_regs->h_video = VI_H_VIDEO_SET(108, 748);
    VI_regs->v_video = VI_V_VIDEO_SET(35, 515);
    VI_regs->v_burst = VI_V_BURST_SET(14, 516);
    VI_regs->x_scale = VI_X_SCALE_SET(320, 640);
    VI_regs->y_scale = VI_Y_SCALE_SET(240, 240);
}

// Forward declare init functions.
bool interrupt_init(void);
bool isviewer_init(void);
void malloc_init(void);

void init_kernel(void)
{
    cop1_init();
    interrupt_init();
    isviewer_init();
    vi_init();
    malloc_init();
}

void assert(bool condition, const char* msg)
{
    if (!condition)
    {
        println(msg);
        abort();
    }
}
