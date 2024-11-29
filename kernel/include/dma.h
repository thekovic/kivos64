#ifndef KIVOS64_DMA_H
#define KIVOS64_DMA_H

#include "intdef.h"

volatile int dma_busy(void);

void dma_wait(void);

uint32_t io_read(uint32_t pi_address);

void io_write(uint32_t pi_address, uint32_t data);

#endif
