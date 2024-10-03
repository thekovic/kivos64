#ifndef BOOT_IPL3_H
#define BOOT_IPL3_H

/**
 * This header defines some accesses to the hardware necessary
 * to execute the boot code. It is a subset of a full HAL.
 */

#include "intdef.h"

#define ONE_MB (1024 * 1024)
#define RDRAM_BANK_SIZE (2 * ONE_MB)

// The loader is run directly from RDRAM for speed. We reserve a portion at
// the end of RDRAM for it.
#define RESERVED_LOADER_SIZE (32*1024)

// User segment base, TLB mapped
#define MEM_KUSEG_BASE (0)
// Kernel segment base, directly mapped, cached
#define MEM_KSEG0_BASE (0x80000000)
// Kernel segment base, directly mapped, uncached 
#define MEM_KSEG1_BASE (0xA0000000)
// Supervisor segment base, TLB mapped
#define MEM_KSSEG_BASE (0xC0000000)
// Kernel segment base, TLB mapped
#define MEM_KSEG3_BASE (0xE0000000)
// Turn address in any segment into physical address
#define ADDR_TO_PHYS(addr) (addr & 0x1FFFFFFF)

// Physical base address of the cartridge ROM space.
#define CART_ROM_BASE (0x10000000)

// Memory-mapped address for accessing PIF RAM (https://n64brew.dev/wiki/PIF-NUS#RAM-based_communication_protocol)
#define PIF_RAM_ADDR (0x1FC007C0)
#define PIF_RAM_CMD_OFFSET (0x3F)
#define PIF_CMD_BOOT_TERMINATE (0x8)

#define C0_WRITE_CAUSE(x)   asm volatile("mtc0 %0,$13"::"r"(x))
#define C0_WRITE_COUNT(x)   asm volatile("mtc0 %0,$9"::"r"(x))
#define C0_WRITE_COMPARE(x) asm volatile("mtc0 %0,$11"::"r"(x))
#define C0_WRITE_WATCHLO(x) asm volatile("mtc0 %0,$18"::"r"(x))

#define PI_DRAM_ADDR       ((volatile uint32_t*) 0xA4600000)  // PI DMA: DRAM address register
#define PI_CART_ADDR       ((volatile uint32_t*) 0xA4600004)  // PI DMA: cartridge address register
#define PI_RD_LEN          ((volatile uint32_t*) 0xA4600008)  // PI DMA: read length register
#define PI_WR_LEN          ((volatile uint32_t*) 0xA460000C)  // PI DMA: write length register
#define PI_STATUS          ((volatile uint32_t*) 0xA4600010)
#define PI_STATUS_DMA_BUSY ( 1 << 0 )
#define PI_STATUS_IO_BUSY  ( 1 << 1 )

#define SI_STATUS          ((volatile uint32_t*) 0xA4800018)
#define SI_STATUS_DMA_BUSY ( 1 << 0 )
#define SI_STATUS_IO_BUSY  ( 1 << 1 )

#define SP_RSP_ADDR     ((volatile uint32_t*) 0xA4040000)
#define SP_DRAM_ADDR    ((volatile uint32_t*) 0xA4040004)
#define SP_RD_LEN       ((volatile uint32_t*) 0xA4040008)
#define SP_WR_LEN       ((volatile uint32_t*) 0xA404000c)
#define SP_STATUS       ((volatile uint32_t*) 0xA4040010)
#define SP_DMA_FULL     ((volatile uint32_t*) 0xA4040014)
#define SP_DMA_BUSY     ((volatile uint32_t*) 0xA4040018)
#define SP_SEMAPHORE    ((volatile uint32_t*) 0xA404001c)
#define SP_PC           ((volatile uint32_t*) 0xA4080000)
#define SP_DMEM_ADDR    ((uint32_t*) 0xA4000000)
#define SP_DMEM_SIZE    (4096)
#define SP_IMEM_ADDR    ((uint32_t*) 0xA4001000)
#define SP_IMEM_SIZE    (4096)

#define SP_WSTATUS_CLEAR_HALT        (1 << 0)  // SP_STATUS write mask: clear HALT bit
#define SP_WSTATUS_SET_HALT          (1 << 1)  // SP_STATUS write mask: set HALT bit
#define SP_WSTATUS_CLEAR_BROKE       (1 << 2)  // SP_STATUS write mask: clear BROKE bit
#define SP_WSTATUS_CLEAR_INTR        (1 << 3)  // SP_STATUS write mask: clear INTR bit
#define SP_WSTATUS_SET_INTR          (1 << 4)  // SP_STATUS write mask: set INTR bit
#define SP_WSTATUS_CLEAR_SSTEP       (1 << 5)  // SP_STATUS write mask: clear SSTEP bit
#define SP_WSTATUS_SET_SSTEP         (1 << 6)  // SP_STATUS write mask: set SSTEP bit
#define SP_WSTATUS_CLEAR_INTBREAK    (1 << 7)  // SP_STATUS write mask: clear INTBREAK bit
#define SP_WSTATUS_SET_INTBREAK      (1 << 8)  // SP_STATUS write mask: set INTBREAK bit
#define SP_WSTATUS_CLEAR_SIG0        (1 << 9)  // SP_STATUS write mask: clear SIG0 bit
#define SP_WSTATUS_SET_SIG0          (1 << 10) // SP_STATUS write mask: set SIG0 bit
#define SP_WSTATUS_CLEAR_SIG1        (1 << 11) // SP_STATUS write mask: clear SIG1 bit
#define SP_WSTATUS_SET_SIG1          (1 << 12) // SP_STATUS write mask: set SIG1 bit
#define SP_WSTATUS_CLEAR_SIG2        (1 << 13) // SP_STATUS write mask: clear SIG2 bit
#define SP_WSTATUS_SET_SIG2          (1 << 14) // SP_STATUS write mask: set SIG2 bit
#define SP_WSTATUS_CLEAR_SIG3        (1 << 15) // SP_STATUS write mask: clear SIG3 bit
#define SP_WSTATUS_SET_SIG3          (1 << 16) // SP_STATUS write mask: set SIG3 bit
#define SP_WSTATUS_CLEAR_SIG4        (1 << 17) // SP_STATUS write mask: clear SIG4 bit
#define SP_WSTATUS_SET_SIG4          (1 << 18) // SP_STATUS write mask: set SIG4 bit
#define SP_WSTATUS_CLEAR_SIG5        (1 << 19) // SP_STATUS write mask: clear SIG5 bit
#define SP_WSTATUS_SET_SIG5          (1 << 20) // SP_STATUS write mask: set SIG5 bit
#define SP_WSTATUS_CLEAR_SIG6        (1 << 21) // SP_STATUS write mask: clear SIG6 bit
#define SP_WSTATUS_SET_SIG6          (1 << 22) // SP_STATUS write mask: set SIG6 bit
#define SP_WSTATUS_CLEAR_SIG7        (1 << 23) // SP_STATUS write mask: clear SIG7 bit
#define SP_WSTATUS_SET_SIG7          (1 << 24) // SP_STATUS write mask: set SIG7 bit

// Writes to all the clear bits in SP_STATUS register and sets HALT bit to stop RSP.
#define SP_CLEAR_STATUS (SP_WSTATUS_SET_HALT | SP_WSTATUS_CLEAR_BROKE | SP_WSTATUS_CLEAR_INTR | \
                 SP_WSTATUS_CLEAR_SIG0 | SP_WSTATUS_CLEAR_SIG1 | \
                 SP_WSTATUS_CLEAR_SIG2 | SP_WSTATUS_CLEAR_SIG3 | \
                 SP_WSTATUS_CLEAR_SIG4 | SP_WSTATUS_CLEAR_SIG5 | \
                 SP_WSTATUS_CLEAR_SIG6 | SP_WSTATUS_CLEAR_SIG7)

#define MI_MODE                       ((volatile uint32_t*) 0xA4300000)
#define MI_WMODE_REPEAT_COUNT(n)      (((n) - 1) & 0x7F)
#define MI_WMODE_CLEAR_REPEAT         (1 << 7)
#define MI_WMODE_SET_REPEAT           (1 << 8)
#define MI_WMODE_CLEAR_DP             (1 << 11)
#define MI_WMODE_CLEAR_UPPER          (1 << 12)
#define MI_WMODE_SET_UPPER            (1 << 13)
#define MI_VERSION                    ((volatile uint32_t*) 0xA4300004)
#define MI_INTERRUPT                  ((volatile uint32_t*) 0xA4300008)
#define MI_INTERRUPT_SP               (1 << 0)
#define MI_INTERRUPT_SI               (1 << 1)
#define MI_INTERRUPT_AI               (1 << 2)
#define MI_INTERRUPT_VI               (1 << 3)
#define MI_INTERRUPT_PI               (1 << 4)
#define MI_INTERRUPT_DP               (1 << 5)
#define MI_MASK                       ((volatile uint32_t*) 0xA430000C)
#define MI_WMASK_CLEAR_SP             (1 << 0)
#define MI_WMASK_SET_SP               (1 << 1)
#define MI_WMASK_CLEAR_SI             (1 << 2)
#define MI_WMASK_SET_SI               (1 << 3)
#define MI_WMASK_CLEAR_AI             (1 << 4)
#define MI_WMASK_SET_AI               (1 << 5)
#define MI_WMASK_CLEAR_VI             (1 << 6)
#define MI_WMASK_SET_VI               (1 << 7)
#define MI_WMASK_CLEAR_PI             (1 << 8)
#define MI_WMASK_SET_PI               (1 << 9)
#define MI_WMASK_CLEAR_DP             (1 << 10)
#define MI_WMASK_SET_DP               (1 << 11)

// Clear all interrupt masks in MI_MASK register.
#define MI_CLEAR_MASK (MI_WMASK_CLEAR_SP | MI_WMASK_CLEAR_SI | MI_WMASK_CLEAR_AI | MI_WMASK_CLEAR_VI | MI_WMASK_CLEAR_PI | MI_WMASK_CLEAR_DP)

#define AI_STATUS                     ((volatile uint32_t*) 0xA450000C)

#define RI_MODE                       ((volatile uint32_t*) 0xA4700000)
#define RI_CONFIG                     ((volatile uint32_t*) 0xA4700004)
#define RI_CURRENT_LOAD               ((volatile uint32_t*) 0xA4700008)
#define RI_SELECT                     ((volatile uint32_t*) 0xA470000C)
#define RI_REFRESH                    ((volatile uint32_t*) 0xA4700010)
#define RI_LATENCY                    ((volatile uint32_t*) 0xA4700014)
#define RI_ERROR                      ((volatile uint32_t*) 0xA4700018)
#define RI_BANK_STATUS                ((volatile uint32_t*) 0xA470001C)

#define PI_CLEAR_INTERRUPT            (1 << 1)
#define SI_CLEAR_INTERRUPT            (0)
#define AI_CLEAR_INTERRUPT            (0)

#define CACHE_INST                      (0)
#define CACHE_INST_SIZE                 (16 * 1024)
#define CACHE_INST_LINESIZE             (32)
#define CACHE_DATA                      (1)
#define CACHE_DATA_SIZE                 (8 * 1024)
#define CACHE_DATA_LINESIZE             (16)
// Cache ops described in manual on page 404
#define INDEX_INVALIDATE                (0 << 2)
#define INDEX_LOAD_TAG                  (1 << 2)
#define INDEX_STORE_TAG                 (2 << 2)
#define INDEX_CREATE_DIRTY              (3 << 2)
#define HIT_INVALIDATE                  (4 << 2)
#define HIT_WRITEBACK_INVALIDATE        (5 << 2)
#define HIT_WRITEBACK                   (6 << 2)

#define BUILD_CACHE_OP(o,c)             ((o) | (c))

#define cache_op(addr, op, linesize, length) ({ \
    { \
        void* cur = (void*) ((unsigned long) addr & ~((linesize) - 1)); \
        int count = (int) ((length) + ((addr) - cur)); \
        for (int i = 0; i < count; i += linesize) \
            { asm volatile ("\tcache %0,(%1)\n"::"i" (op), "r" (cur + i)); } \
    } \
})

static inline void data_cache_hit_writeback_invalidate(volatile void* addr, unsigned long length)
{
    cache_op(addr, BUILD_CACHE_OP(HIT_WRITEBACK_INVALIDATE, CACHE_DATA), CACHE_DATA_LINESIZE, length);
}   

// Clear data and instruction caches.
static inline void cop0_clear_cache(void)
{
    // Write 0 to CP0 register TagLo (Cache Tag register low)
    asm volatile ("mtc0 $0, $28");
    // Write 0 to CP0 register TagHi (Cache Tag register high)
    asm volatile ("mtc0 $0, $29");
    cache_op((void*) MEM_KSEG0_BASE, BUILD_CACHE_OP(INDEX_STORE_TAG, CACHE_DATA), CACHE_DATA_LINESIZE, CACHE_DATA_SIZE);
    cache_op((void*) MEM_KSEG0_BASE, BUILD_CACHE_OP(INDEX_STORE_TAG, CACHE_INST), CACHE_INST_LINESIZE, CACHE_INST_SIZE);
}

__attribute__((noreturn))
static inline void abort(void)
{
    while(1) {}
}

// Read one word at a given address.
static inline uint32_t io_read(uint32_t addr)
{
    while (*PI_STATUS & (PI_STATUS_DMA_BUSY | PI_STATUS_IO_BUSY)) {}
    volatile uint32_t* vaddr = (uint32_t*) addr;
    return *vaddr;
}

typedef struct
{
    uint32_t memory_size;
    uint32_t random_seed;
    uint32_t flags;
    uint32_t elf_offset;
} bootinfo_t;

#endif // BOOT_IPL3_H
