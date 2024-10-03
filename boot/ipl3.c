/*
 * This file contains the first stage of the IPL3 boot code. The second stage
 * is stored in loader.c.
 * 
 * The primary goal of this task is to perform RDRAM initialization.
 * In fact, after a cold boot the RDRAM chips themselves need to be configured.
 * However, this complex task has been omitted here for practical reasons
 * because emulators don't simulate RDRAM at this time and flashcarts
 * (such as Everdrive 64 X7) used for testing this code on real hardware
 * boot ROMs from their menu which handles RDRAM initialization already.
 * Therefore, this IPL3 limits itself to just clearing the RDRAM to zero.
 * 
 * After RAM is inizialized, we copy the second stage of IPL3 (loader.c) from
 * DMEM to the end of RDRAM, and jump to it to continue with the execution.
 * This is done mainly because running from RDRAM is faster than from DMEM,
 * so there is no reason in staying in DMEM after RDRAM is available.
 * 
 * Layout of ROM
 * =============
 * 
 * The following table details how a ROM using this IPL3 boot code is laid out.
 * 
 * Development layout:
 * 0x0000 - HEADER
 * 0x0040 - Signed IPL3 Trampoline
 * 0x1040 - IPL3 code proper (unsigned)
 * 0x2000 - Rompak TOC
 * ...... - Main ELF file
 * 
 * The ROM header is a 64-byte structure that contains information about the
 * ROM. It is not used by IPL3 itself, and is normally just advisory (that is,
 * might be used by flashcart menus to identify a game, provide the game tile,
 * etc.).
 * 
 * The signed IPL3 trampoline is a small piece of code that is used to load
 * the actual IPL3 from offset 0x1040 into DMEM. The only known way to obtain
 * a valid IPL3 signature is using brute force with powerful current GPUs which
 * means it is inconvenient to sign each build of IPL3.
*/

#include "intdef.h"
#include "ipl3.h"

// These registers contain boot flags passed by IPL2. Define them globally
// during the first stage of IPL3, so that the registers are not reused.
register uint32_t ipl2_romType   asm ("s3");
register uint32_t ipl2_tvType    asm ("s4");
register uint32_t ipl2_resetType asm ("s5");
register uint32_t ipl2_romSeed   asm ("s6");
register uint32_t ipl2_version   asm ("s7");

// Structure for standard N64 ROM header.
// Documented at https://n64brew.dev/wiki/ROM_Header#Standard_header
typedef struct __attribute__((packed))
{
    uint32_t pi_dom1_config;
    uint32_t clock_rate;
    uint32_t boot_address;
    uint32_t sdk_version;
    uint64_t checksum;
    uint64_t reserved1;
    char title[20];
    char reserved2[7];
    uint32_t gamecode;
    uint8_t rom_version;
} rom_header_t;

_Static_assert(sizeof(rom_header_t) == 64, "invalid sizeof(rom_header_t)");

__attribute__((section(".header"), used))
const rom_header_t header = {
    // Standard PI DOM1 config
    .pi_dom1_config = 0x80371240,
    // Default title name
    .title = "KIVOS64             ",
};

// Clear 64 bits (8 bytes) of memory regardless of address alignment.
static void bzero64(void* mem)
{
    asm volatile ("sdl $0, 0(%0); sdr $0, 7(%0);" :: "r"(mem));
}

// Clear memory in RSP using CPU.
static void rsp_bzero(void)
{
    while (*SP_DMA_BUSY) {}

    // Use CPU to clear IMEM.
    for (int i = 0; i < (SP_IMEM_SIZE / 4); i++)
    {
        // Use just plain write instead of the 64-bit bzero because DMEM/IMEM
        // can't be used with 64-bit instructions.
        SP_IMEM_ADDR[i] = 0;
    }
}

// Clear memory using CPU.
void rdram_bzero(uint32_t rdram, int size)
{
    // Make sure we use uncached access.
    rdram |= MEM_KSEG1_BASE;
    do
    {
        bzero64((void*) rdram);
        rdram += 8;
        size -= 8;
    } while (size > 8);
}

// This function is placed by the linker script immediately above the stage1()
// function. We just change the stack pointer here, as very first thing.
// This action is separate so that the rest of stage1 has the stack frame set up.
__attribute__((noreturn, section(".stage1.pre")))
void stage1pre(void)
{
    // Move the stack to DMEM.
    asm volatile ("li $sp, %0"::"i"((uint32_t) SP_DMEM_ADDR + SP_DMEM_SIZE - 0x10));
    // Avoid function epilog, fallthrough to stage1.
    __builtin_unreachable();
}

__attribute__((noreturn, section(".stage1")))
void stage1(void)
{
    // Reset important CP0 registers
    C0_WRITE_CAUSE(0);
    C0_WRITE_COUNT(0);
    C0_WRITE_COMPARE(0);
	C0_WRITE_WATCHLO(0);

    int memsize;

    // Cold boots.
    // Doesn't happen on emulators or on Everdrive (because the menu handles cold boot),
    // therefore not implemented.
    if (*RI_SELECT == 0)
    {
        memsize = 0;
    }
    // Warm boots (with Reset button).
    else
    {
        memsize = 0;
        // Test memory banks for read/write.
        for (int chip_id = 0; chip_id < 8; chip_id += 2)
        {
            volatile uint32_t* ptr = (void*) (MEM_KSEG1_BASE + chip_id * ONE_MB);
            // Check with zero.
            ptr[0] = 0;
            if (ptr[0] != 0)
            {
                break;
            }
            // Also check with magic value because maybe the memory was not
            // writable and it gave back zero by chance.
            ptr[0] = 0x12345678;
            if (ptr[0] != 0x12345678)
            {
                break;
            }
            memsize += RDRAM_BANK_SIZE;
        }

        // Clear memory. Skip the first 0x400 bytes of RAM because it
        // historically contains some boot flags that some existing code
        // might expect to stay there.
        // For instance, the Everdrive menu expects 0x80000318 to still
        // contain the RDRAM size after a warm boot, and we need to comply
        // with this even if Everdrive itself doesn't use this IPL3.
        rsp_bzero();
        rdram_bzero(MEM_KSEG1_BASE + 0x400, memsize - 0x400 - RESERVED_LOADER_SIZE);
    }

    // Access the symbol for start of stage2 defined in the linker script.
    extern uint32_t __stage2_start[];
    // Turn it into an address.
    uint32_t stage2_start_addr = (uint32_t) __stage2_start;
    // Read the stage2 size that's saved there during link time (see linker script).
    int stage2_size = io_read(stage2_start_addr);
    // Increase start address by what we just read (+ padding).
    stage2_start_addr += 8;
    
    void* rdram_stage2_addr = (void*) (MEM_KSEG0_BASE + (memsize) - (stage2_size));
    // Set destination for DMA.
    *PI_DRAM_ADDR = (uint32_t) rdram_stage2_addr;
    // Set source for DMA.
    *PI_CART_ADDR = ADDR_TO_PHYS(stage2_start_addr);
    // Start DMA to copy stage2 (loader.c) from ROM to the end of RDRAM.
    *PI_WR_LEN = stage2_size - 1;

    // Clear D/I-cache, useful after warm boot.
    cop0_clear_cache();

    // Fill boot information at beginning of DMEM. The rest of IMEM has been
    // cleared by now anyway. Notice that we also store BSS in IMEM, so the
    // linker script reserves initial part of boot information.
    bootinfo_t* bootinfo = (bootinfo_t*) SP_DMEM_ADDR;
    bootinfo->memory_size = memsize;
    bootinfo->random_seed = 0x12345678;
    bootinfo->flags = (ipl2_romType << 24) | (ipl2_tvType << 16) | (ipl2_resetType << 8) | 0;
    bootinfo->elf_offset = 0;

    // Wait until stage2 is fully loaded into RDRAM.
    while (*PI_STATUS & 1) {}

    // Set up stack for stage2.
    asm volatile ("move $sp, %0"::"r"(((void*)(MEM_KSEG0_BASE + (memsize) - (stage2_size)) - 16)));

    // Jump to stage2 in RDRAM.
    goto *rdram_stage2_addr;
}
