#ifndef KIVOS64_SYSTEM_H
#define KIVOS64_SYSTEM_H

#include "intdef.h"

// Memory size as detected by IPL3.
extern int __boot_memsize;
// Random seed provided by IPL3.
extern int __boot_random_seed;
// TV type as detected by IPL3.
extern int __boot_tvtype;
// Reset type as detected by IPL3.
extern int __boot_resettype;

#define SP_DMEM_ADDR    ((volatile uint32_t*) 0xA4000000)

// User segment base, TLB mapped.
#define MEM_KUSEG_BASE (0)
// Kernel segment base, directly mapped, cached.
#define MEM_KSEG0_BASE (0x80000000)
// Kernel segment base, directly mapped, uncached .
#define MEM_KSEG1_BASE (0xA0000000)
// Supervisor segment base, TLB mapped.
#define MEM_KSSEG_BASE (0xC0000000)
// Kernel segment base, TLB mapped.
#define MEM_KSEG3_BASE (0xE0000000)

// Turn address in any segment into physical address.
#define ADDR_TO_PHYS(addr)  ((addr) & 0x1FFFFFFF)
#define ADDR_TO_KSEG0(addr) ((addr) | MEM_KSEG0_BASE)
#define ADDR_TO_KSEG1(addr) ((addr) | MEM_KSEG1_BASE)

void data_cache_index_writeback_invalidate(volatile void* addr, unsigned long length);

void data_cache_hit_invalidate(volatile void* addr, unsigned long length);

void data_cache_hit_writeback_invalidate(volatile void* addr, unsigned long length);

void data_cache_hit_writeback(volatile void* addr, unsigned long length);

void inst_cache_index_invalidate(volatile void* addr, unsigned long length);

void inst_cache_hit_invalidate(volatile void* addr, unsigned long length);

void inst_cache_hit_writeback(volatile void* addr, unsigned long length);

void print(const char* data);
void println(const char* data);
void println_u32(const char* data, uint32_t value);
void println_x32(const char* data, uint32_t value);
void assert(bool condition, const char* msg);

__attribute__((noreturn))
static inline void abort(void)
{
    while(1) {}
}

#endif
