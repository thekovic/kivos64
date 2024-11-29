#include "dma.h"
#include "pi.h"
#include "system.h"
#include "interrupt.h"

volatile int dma_busy(void)
{
    return PI_regs->status & (PI_STATUS_DMA_BUSY | PI_STATUS_IO_BUSY);
}

void dma_wait(void)
{
    while (dma_busy()) {}
}

uint32_t io_read(uint32_t pi_address)
{
    uint32_t result;
    volatile uint32_t* uncached_address = (uint32_t*) (pi_address | MEM_KSEG1_BASE);

    interrupt_disable();
        dma_wait();
        result = *uncached_address;
    interrupt_enable();

    return result;
}

void io_write(uint32_t pi_address, uint32_t data) 
{
    volatile uint32_t* uncached_address = (uint32_t*) (pi_address | MEM_KSEG1_BASE);

    interrupt_disable();
        dma_wait();
        *uncached_address = data;
    interrupt_enable();
}
