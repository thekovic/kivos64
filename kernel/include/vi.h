#ifndef KIVOS64_VI_H
#define KIVOS64_VI_H

#include "intdef.h"

#define VI_REG_BASE      (0xA4400000)

/** @brief VI_CTRL: Set the framebuffer source as blank (no data and no sync, TV screens will either show static or nothing). */
#define VI_CTRL_TYPE_BLANK                  (0b00)
/** @brief VI_CTRL: Set the framebuffer source as 16-bit (5-5-5-1). */
#define VI_CTRL_TYPE_16_BPP                 (0b10)
/** @brief VI_CTRL: Set the framebuffer source as 32-bit (8-8-8-8). */
#define VI_CTRL_TYPE_32_BPP                 (0b11)
/** @brief VI_CTRL: Adds randomized noise to the video output. */
#define VI_CTRL_GAMMA_DITHER                (1 << 2)
/** @brief VI_CTRL: Fixes non-linear gamma in TV screens. */
#define VI_CTRL_GAMMA                       (1 << 3)
/** @brief VI_CTRL: Fixes minor artifacts left over from anti-aliasing. */
#define VI_CTRL_DIVOT                       (1 << 4)
/** @brief VI_CTRL: Enable interlaced output. */
#define VI_CTRL_SERRATE                     (1 << 6)
/** @brief VI_CTRL: Enable AA and enable resampling, fetch pixels always. */
#define VI_CTRL_AA_RESAMPLE_FETCH_ALWAYS    (0b00 << 8)
/** @brief VI_CTRL: Enable AA and enable resampling, fetch pixels when needed. */
#define VI_CTRL_AA_RESAMPLE_FETCH_NEEDED    (0b01 << 8)
/** @brief VI_CTRL: Disable AA and enable resampling. */
#define VI_CTRL_AA_RESAMPLE                 (0b10 << 8)
/** @brief VI_CTRL: Disable AA and disable resampling. */
#define VI_CTRL_AA_NONE                     (0b11 << 8)
/** @brief VI_CTRL: Default value for pixel advance. */
#define VI_CTRL_PIXEL_ADVANCE_DEFAULT       (0b0011 << 12)
/** @brief VI_CTRL: Enable dedither filter. */
#define VI_CTRL_DEDITHER                    (1 << 16)

/** @brief VI_ORIGIN: Set the address of a framebuffer. */
#define VI_ORIGIN_SET(value)                ((value) & 0xFFFFFF)

/** @brief VI_WIDTH: Set the width of a framebuffer. */
#define VI_WIDTH_SET(value)                 ((value) & 0xFFF)

/** @brief VI_V_CURRENT: Default value for vblank begin line. */
#define VI_V_CURRENT_VBLANK                 (2)

/** @brief VI_V_INTR: Set value for vertical interrupt. */
#define VI_V_INTR_SET(value)                ((value) & 0x3FF)

/** @brief VI_BURST: NTSC default start of color burst in pixels from hsync. */
#define VI_BURST_START_NTSC                 (62)
/** @brief VI_BURST: NTSC default vertical sync width in half lines. */
#define VI_VSYNC_WIDTH_NTSC                 (5)
/** @brief VI_BURST: NTSC default color burst width in pixels. */
#define VI_BURST_WIDTH_NTSC                 (34)
/** @brief VI_BURST: NTSC default horizontal sync width in pixels. */
#define VI_HSYNC_WIDTH_NTSC                 (57)
/** @brief VI_BURST: Set start of color burst in pixels from hsync. */
#define VI_BURST_START(value)               (((value) & 0x3FF) << 20)
/** @brief VI_BURST: Set vertical sync width in half lines. */
#define VI_VSYNC_WIDTH(value)               (((value) & 0xF) << 16)
/** @brief VI_BURST: Set color burst width in pixels. */
#define VI_BURST_WIDTH(value)               (((value) & 0xFF) << 8)
/** @brief VI_BURST: Set horizontal sync width in pixels. */
#define VI_HSYNC_WIDTH(value)               (((value) & 0xFF) << 0)
/** @brief VI_BURST: Set all values. */
#define VI_H_BURST_SET(burst_start, vsync_width, burst_width, hsync_width) \
    (VI_BURST_START(burst_start) | VI_VSYNC_WIDTH(vsync_width) | \
     VI_BURST_WIDTH(burst_width) | VI_HSYNC_WIDTH(hsync_width))

/** @brief VI_V_TOTAL: Set the total number of visible and non-visible half-lines. */
#define VI_V_TOTAL_SET(value)               ((value) & 0x3FF)

/** @brief VI_H_TOTAL: Set the total width of a line in quarter-pixel units and the 5-bit leap pattern. */
#define VI_H_TOTAL_SET(leap_pattern, hsync) ((((leap_pattern) & 0x1F) << 16) | ((hsync) & 0xFFF))

/** @brief VI_H_TOTAL_LEAP: Set alternate scanline length for one scanline during vsync, leap_a and leap_b are selected based on the leap pattern in VI_H_SYNC. */
#define VI_H_TOTAL_LEAP_SET(leap_a, leap_b) ((((leap_a) & 0xFFF) << 16) | ((leap_b) & 0xFFF))

/** @brief VI_H_VIDEO: Set the horizontal start and end of the active video area, in screen pixels. */
#define VI_H_VIDEO_SET(start, end)          ((((start) & 0x3FF) << 16) | ((end) & 0x3FF))

/** @brief VI_V_VIDEO: Set the vertical start and end of the active video area, in half-lines */
#define VI_V_VIDEO_SET(start, end)          ((((start) & 0x3FF) << 16) | ((end) & 0x3FF))

/** @brief VI_V_BURST: Set the start and end of color burst from vsync, in half-lines */
#define VI_V_BURST_SET(start, end)          ((((start) & 0x3FF) << 16) | ((end) & 0x3FF))

/** @brief VI_X_SCALE: Set 1/horizontal scale factor (value is converted to 2.10 format). */
#define VI_X_SCALE_SET(from, to)            (((1024 * (from)) + ((to) / 2)) / (to))

/** @brief VI_Y_SCALE: Set 1/vertical scale factor (value is converted to 2.10 format). */
#define VI_Y_SCALE_SET(from, to)            (((1024 * (from)) + ((to) / 2)) / (to))

typedef struct VI_registers_s
{
    /** @brief Register controlling general display filter configuration. */
    uint32_t control;
    /** @brief RDRAM base address of the video output framebuffer. This can be changed as needed to implement double or triple buffering. */
    uint32_t origin;
    /** @brief Width of the framebuffer in pixels. */
    uint32_t width;
    /** @brief When this registers equals the value of #current_line, interrupt is triggered. */
    uint32_t v_interrupt;
    /** @brief Current half line, sampled once per line. */
    uint32_t v_current;
    /** @brief Sync and color burst values to setup the video signal. */
    uint32_t h_burst;
    /** @brief One less than the total number of visible and non-visible half-lines. This should match either NTSC (progressive: 525, interlaced: 524) or PAL (progressive: 625, interlaced: 624). */
    uint32_t v_total;
    /** @brief One less than the total length of a scanline in 1/4 pixel units. Should always use standard values: NTSC (3093) or PAL (3177). */
    uint32_t h_total;
    /** @brief Specifies an alternate scanline length for one scanline during vsync. */
    uint32_t h_total_leap;
    /** @brief Start/end of the active video image horizontally, in screen pixels. */
    uint32_t h_video;
    /** @brief Start/end of the active video image vertically, in screen pixels. */
    uint32_t v_video;
    /** @brief Start/end of vertical color burst values to setup the video signal. */
    uint32_t v_burst;
    /** @brief Horizontal scale factor of the active video image. */
    uint32_t x_scale;
    /** @brief Vertical scale factor of the active video image. */
    uint32_t y_scale;
} VI_registers_t;

extern volatile VI_registers_t* const VI_regs;

#endif
