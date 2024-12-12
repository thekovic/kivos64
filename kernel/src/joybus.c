#include "si.h"
#include "system.h"
#include "interrupt.h"
#include "memory.h"

// Joybus protocol packet size in bytes.
#define JOYBUS_PACKET_SIZE          (64)
// Joybus protocol packet size in words.
#define JOYBUS_PACKET_WORDS         (JOYBUS_PACKET_SIZE / sizeof(uint32_t))

// Command is composed as following:
// FF - Padding.
// 01 - Number of bytes in the question part.
// 04 - Number of bytes in the answer part.
// 01 - Question, here contains command ID 0x01 which is the controller state command.
// FFFFFFFF - 4 bytes of padding to make space for the answer.
#define JOYBUS_CMD_READ_CONTROLLER      (0xFF010401)
// Byte FE terminates the Joybus packet. Rest is padding to word size.
#define JOYBUS_CMD_TERMINATE_PACKET     (0xFE000000)
// Size of one command in words.
#define JOYBUS_CMD_WORDS                (2)

/** @brief Joybus temporary output buffer. */
static uint32_t joybus_buffer[JOYBUS_PACKET_WORDS] __attribute__((aligned(16)));

uint32_t next = 0;

/** @brief Receive a Joybus reply from the PIF and write it into a buffer in RAM. */
void __joypad_callback()
{
    assert((SI_regs->status & (SI_STATUS_DMA_BUSY | SI_STATUS_IO_BUSY)) == 0, "__joypad_callback: Called when SI is busy.");

    // Invalidate the buffer in cache so that we actually update the contents.
    data_cache_hit_invalidate(joybus_buffer, JOYBUS_PACKET_SIZE);
    SI_regs->ram_address = (uint32_t) joybus_buffer;
    SI_regs->pif_ram_read = (uint32_t) PIF_RAM_ADDR;
}

/** @brief Read Joybus reply after SI DMA finishes. */
void __controller_callback()
{
    memcpy((void*) &next, joybus_buffer + 1, sizeof(uint32_t));
        
    if (next != 0)
    {
        println_x32("pressed = ", next);
    }
}

/** @brief Initialize the joybus module. */
void joybus_init(void)
{
    // Wait for any pending SI write. This can happen mainly because of the
    // write made by entrypoint.S to complete the PIF boot. If the write is still
    // pending, it would trigger a SI interrupt later and cause a crash.
    while (SI_regs->status & (SI_STATUS_DMA_BUSY | SI_STATUS_IO_BUSY)) {}

    // Acknowledge any pending SI interrupt.
    SI_regs->status = 0;

    // Prepare Joybus command to read controller state.
    volatile uint32_t* pif_ram = (volatile uint32_t*) (MEM_KSEG1_BASE + PIF_RAM_ADDR);
    pif_ram[0] = JOYBUS_CMD_READ_CONTROLLER;
    while (SI_regs->status & (SI_STATUS_DMA_BUSY | SI_STATUS_IO_BUSY)) {}
    
    pif_ram[JOYBUS_CMD_WORDS] = JOYBUS_CMD_TERMINATE_PACKET;
    while (SI_regs->status & (SI_STATUS_DMA_BUSY | SI_STATUS_IO_BUSY)) {}
    
    pif_ram[JOYBUS_PACKET_WORDS - 1] = PIF_CMD_JOYBUS_START;
    while (SI_regs->status & (SI_STATUS_DMA_BUSY | SI_STATUS_IO_BUSY)) {}

    // Activate SI interrupt.
    interrupt_set_SI(true);
}
