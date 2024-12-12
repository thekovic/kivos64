#ifndef KIVOS64_SI_H
#define KIVOS64_SI_H

#include "intdef.h"

#define SI_REG_BASE      (0xA4800000)

#define PIF_RAM_ADDR (0x1FC007C0)
#define PIF_RAM_CMD_OFFSET (0x3F)

#define SI_STATUS_DMA_BUSY      (1 << 0)
#define SI_STATUS_IO_BUSY       (1 << 1)
#define SI_STATUS_READ_PENDING  (1 << 2)
#define SI_STATUS_DMA_ERROR     (1 << 3)
#define SI_STATUS_INTERRUPT     (1 << 12)

typedef struct SI_registers_s
{
    /** @brief Address in RDRAM. */
    uint32_t ram_address;
    /** @brief Address where to read data in PIF_RAM. Writing to this register initiates DMA. */
    uint32_t pif_ram_read;
    uint32_t reserved1;     // 0x04800008
    uint32_t reserved2;     // 0x0480000C
    /** @brief Address where to write data to PIF_RAM. Writing to this register initiates DMA. */
    uint32_t pif_ram_write;
    uint32_t reserved3;     // 0x04800014
    /** @brief Status register of the SI. */
    uint32_t status;
} SI_registers_t;

extern volatile SI_registers_t* const SI_regs;

#endif
