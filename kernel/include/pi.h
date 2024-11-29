#ifndef KIVOS64_PI_H
#define KIVOS64_PI_H

#include "intdef.h"

#define PI_REG_BASE      (0xA4600000)

// Base address of the SRAM save space in PI address space.
#define PI_CART_ROM_BASE (0x08000000)
// Base address of the cartridge ROM space in PI address space.
#define PI_CART_ROM_BASE (0x10000000)

// Read bits.
#define PI_STATUS_DMA_BUSY      (1 << 0)
#define PI_STATUS_IO_BUSY       (1 << 1)
#define PI_STATUS_DMA_ERROR     (1 << 2)
#define PI_STATUS_DMA_DONE      (1 << 3)
// Write bits.
#define PI_STATUS_SET_DMA_RESET (1 << 0)
#define PI_STATUS_CLR_INTERRUPT (1 << 0)

typedef struct PI_registers_s
{
    /** @brief Uncached address in RDRAM, must be 8 byte aligned. */
    uint32_t ram_address;
    /** @brief Address of data in PI address space, must be 2 byte aligned. */
    uint32_t pi_address;
    /** @brief
     * How much data to read from RAM into the peripheral, must be a multiple of 2.
     * Writing to this register will start DMA transfer.
     */
    uint32_t read_length;
    /** @brief
     * How much data to write to RAM from the peripheral, must be a multiple of 2.
     * Writing to this register will start DMA transfer.
     */
    uint32_t write_length;
    /** @brief Status of the PI, including DMA busy */
    uint32_t status;
    // TODO: add remaining registers
} PI_registers_t;

extern volatile PI_registers_t* const PI_regs;

#endif
