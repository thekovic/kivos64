/**
 * This module implements the second stage of the loader, which is responsible
 * of searching and loading the ELF file embedded in the ROM, and jumping
 * to the entrypoint.
 * 
 * This stage runs from "high RDRAM", that is, it is placed at the end of RDRAM.
 * The code is compiled to be relocatable via a trick in the Makefile, so that
 * it can be placed at dynamic addresses.
 * 
 * The tasks performed by this stage are:
 * 
 *  * Find the ELF file in ROM.
 *  * Load the ELF file in memory (PT_LOAD segments).
 *  * Reset the RCP hardware (SP, DP, MI, PI, SI, AI).
 *  * Notify the PIF that the boot process is finished.
 *  * Clear DMEM (except the boot flags area).
 *  * Jump to the entrypoint.
 */

#include "intdef.h"
#include "ipl3.h"

// Magic word to identify start of ELF header.
#define ELF_MAGIC (0x7F454C46)
// Definition for loadable program segment in ELF.
#define PT_LOAD (0x1)
// Size of ELF program header.
#define PHDR_SIZE (0x20)

__attribute__((far, noreturn))
void stage3(uint32_t entrypoint);

static inline void pi_wait(void)
{
    while (*PI_STATUS & (PI_STATUS_DMA_BUSY | PI_STATUS_IO_BUSY)) {}
}

// Perform DMA from cartridge.
static void pi_read_async(uint32_t rdram_addr, uint32_t cart_addr, uint32_t len)
{
    pi_wait();
    // Set destination for DMA.
    *PI_DRAM_ADDR = rdram_addr;
    // Set source for DMA.
    *PI_CART_ADDR = cart_addr;
    // Start DMA by writing length minus 1.
    *PI_WR_LEN = len - 1;
}

static uint32_t io_read32(uint32_t addr)
{
    while (*PI_STATUS & (PI_STATUS_DMA_BUSY | PI_STATUS_IO_BUSY)) {}
    addr |= MEM_KSEG1_BASE;
    volatile uint32_t* vaddr = (uint32_t*) addr;
    return *vaddr;
}

static uint16_t io_read16(uint32_t addr)
{
    uint32_t addr_align32 = addr & ~0x3;
    uint32_t value = io_read32(addr_align32);
    bool is_low = !(addr & 2);
    return (is_low) ? value >> 16 : value;
}

static uint8_t io_read8(uint32_t addr)
{
    uint32_t addr_align32 = addr & ~0x3;
    uint32_t value = io_read32(addr_align32);
    uint32_t bit_offset = (~addr & 3) * 8;
    return value >> bit_offset;
}

// Reset RCP status.
static inline void rcp_reset(void)
{
    *SP_STATUS = SP_CLEAR_STATUS;
    *SP_SEMAPHORE = 0;
    *SP_PC = 0;
    
    *MI_MASK = MI_CLEAR_MASK;
    *MI_INTERRUPT = 0;
    
    *PI_STATUS = PI_CLEAR_INTERRUPT;
    *SI_STATUS = SI_CLEAR_INTERRUPT;
    *AI_STATUS = AI_CLEAR_INTERRUPT;
    *MI_MODE = MI_WMODE_CLEAR_DP;
}

// Inform PIF that the boot process is finished. If this is not written,
// the PIF will halt the CPU after 5 seconds. 
static inline void pif_terminate_boot(void)
{
    // Align access to CMD byte to word because we are making a 32-bit access on big endian.
    volatile uint32_t* vaddr = (volatile uint32_t*) ((MEM_KSEG1_BASE + PIF_RAM_ADDR + PIF_RAM_CMD_OFFSET) & ~0x3);
    while (*SI_STATUS & (SI_STATUS_DMA_BUSY | SI_STATUS_IO_BUSY)) {}
    *vaddr = PIF_CMD_BOOT_TERMINATE;
}

__attribute__((used))
void stage2(void)
{
    uint32_t elf_header_addr = CART_ROM_BASE + 0x1000;
    bool found_elf = false;

    // Search for the ELF header. We search for a 256-byte aligned header
    // starting at offset 0x1000 in the ROM area (after the IPL3).
    // We search for 64 MiB of ROM space (takes only a couple of seconds)
    for (int i = 0; i < 64 * ONE_MB / 256; i++)
    {
        if (io_read32(elf_header_addr) == ELF_MAGIC)
        {
            found_elf = true;
            break;
        }
        elf_header_addr += 0x100;
    }

    if (!found_elf)
    {
        abort();
    }

    // Store the ELF offset in the boot flags
    bootinfo_t* bootinfo = (bootinfo_t*) SP_DMEM_ADDR;
    bootinfo->elf_offset = elf_header_addr;

    // ELF structure followed from https://en.wikipedia.org/wiki/Executable_and_Linkable_Format#ELF_header

    // Check if the ELF is 32/64 bit, and if it's big/little endian
    uint8_t elf_type = io_read8(elf_header_addr + 0x4);
    if (elf_type == 2)
    {
        abort();
    }
    uint8_t elf_endian = io_read8(elf_header_addr + 0x5);
    if (elf_endian == 1)
    {
        abort();
    }

    // Read ELF header for offset and number of program headers.
    uint32_t phoff = io_read32(elf_header_addr + 0x1C);
    int phnum = io_read16(elf_header_addr + 0x2C);
    uint32_t entrypoint = io_read32(elf_header_addr + 0x18);

    // Load the program segments
    for (int i = 0; i < phnum; i++)
    {
        uint32_t phdr[PHDR_SIZE];
        // Invalidate cache entry to make sure we read the next header properly
        data_cache_hit_writeback_invalidate(phdr, PHDR_SIZE);
        // Load program header
        pi_read_async((uint32_t) phdr, elf_header_addr + phoff + (PHDR_SIZE * i), PHDR_SIZE);
        pi_wait();

        uint32_t type = phdr[0];
        uint32_t offset = phdr[1];
        uint32_t vaddr = phdr[2];
        uint32_t size = phdr[4];

        // Ignore non-loadable segments
        if (type != PT_LOAD)
        {
            continue;
        }

        // Ignore empty segments
        if (!size)
        {
            continue;
        }

        // Make sure we can do PI DMA
        if ((vaddr % 8) != 0)
        {
            abort();
        }
        if ((offset % 2) != 0)
        {
            abort();
        }

        // Load the segment into RDRAM. Notice that we don't need to clear
        // extra size at the end of the segment (as specified by phdr[5])
        // as the whole RDRAM has been cleared already.
        pi_read_async(vaddr, elf_header_addr + offset, size);

        // Wait for the DMA to finish.
        pi_wait();
    }

    // Reset the RCP hardware
    rcp_reset();

    // Jump to the ROM finish function
    stage3(entrypoint);
}

// This is the last stage of IPL3. It runs directly from ROM so that we are
// free of cleaning up our breadcrumbs in both DMEM and RDRAM.
__attribute__((far, noreturn, section(".stage3")))
void stage3(uint32_t entrypoint)
{
    // Notify the PIF that the boot process is finished.
    pif_terminate_boot();

    // Reset the CPU cache, so that the application starts from a pristine state.
    cop0_clear_cache();

    // Read memory size from boot flags.
    int memsize = *(volatile uint32_t*) SP_DMEM_ADDR;

    // Clear the reserved portion of RDRAM.
    while (*SP_DMA_FULL) {}
    *SP_RSP_ADDR = (uint32_t) SP_IMEM_ADDR;
    *SP_DRAM_ADDR = memsize - RESERVED_LOADER_SIZE;
    *SP_WR_LEN = RESERVED_LOADER_SIZE - 1;

    // Clear DMEM (leave only the boot flags area intact).
    while (*SP_DMA_FULL) {}
    *SP_RSP_ADDR = (uint32_t) SP_DMEM_ADDR + sizeof(bootinfo_t);
    // Area above 8 MiB which is guaranteed to be empty.
    *SP_DRAM_ADDR = 0x00802000;  
    *SP_RD_LEN = SP_DMEM_SIZE - sizeof(bootinfo_t) - 1;

    // Wait until the PIF is done. This will also clear the interrupt, so that
    // we don't leave the interrupt pending when we go to the entrypoint.
    while (*SI_STATUS & (SI_STATUS_DMA_BUSY | SI_STATUS_IO_BUSY)) {}
    *SI_STATUS = SI_CLEAR_INTERRUPT;

    // Wait for RSP DMA to finish.
    while (*SP_DMA_BUSY) {}

    // Configure SP at the end of RDRAM. This is a good default in general,
    // then of course userspace code is free to reconfigure it.
    asm ("move $sp, %0" : : "r" (0x80000000 + memsize - 0x10));

    goto *(void*)entrypoint;
}
