#include "system.h"
#include "cop0.h"
#include "mi.h"
#include "pi.h"
#include "vi.h"

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

void vi_init(void)
{
    *VI_regs = (VI_registers_t) {
        .control = (VI_CTRL_TYPE_32_BPP | VI_CTRL_AA_RESAMPLE | VI_CTRL_PIXEL_ADVANCE_DEFAULT),
        .origin = VI_ORIGIN_SET(0),
        .width = VI_WIDTH_SET(0),
        .v_interrupt = VI_V_INTR_SET(2),
        .v_current = 0,
        .h_burst = VI_H_BURST_SET(VI_BURST_START_NTSC, VI_VSYNC_WIDTH_NTSC, VI_BURST_WIDTH_NTSC, VI_HSYNC_WIDTH_NTSC),
        .v_total = VI_V_TOTAL_SET(525),
        .h_total = VI_H_TOTAL_SET(0, 3093),
        .h_total_leap = VI_H_TOTAL_LEAP_SET(3093, 3093),
        .h_video = VI_H_VIDEO_SET(108, 748),
        .v_video = VI_V_VIDEO_SET(35, 515),
        .v_burst = VI_V_BURST_SET(14, 516),
        .x_scale = VI_X_SCALE_SET(0, 640),
        .y_scale = VI_Y_SCALE_SET(0, 240),
    };
}
