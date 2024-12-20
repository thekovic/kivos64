#include "cop0.h"
#include "system.h"

#define TLB_GLOBAL          (1 << 0) // If set, address space identifier is ignored and all processes can use this entry.
#define TLB_VALID           (1 << 1) // If set, TLB (dual-)entry is valid and can be used. 
#define TLB_WRITE_ENABLE    (1 << 2) // If set, mapped memory can be written to (unset means read-only).
#define TLB_UNCACHED        (1 << 4) // If set, access to this memory is uncached.

#define TLB_PAGE_SIZE_4KB   (0x0000 << 13)
#define TLB_PAGE_SIZE_16KB  (0x0003 << 13)
#define TLB_PAGE_SIZE_64KB  (0x000F << 13)
#define TLB_PAGE_SIZE_256KB (0x003F << 13)
#define TLB_PAGE_SIZE_1MB   (0x00FF << 13)
#define TLB_PAGE_SIZE_4MB   (0x03FF << 13)
#define TLB_PAGE_SIZE_16MB  (0x0FFF << 13)

#define PAGE_SIZE           (256 * 1024)
#define USEG_VADDR          (0x00080000)
#define USEG_PADDR          (0x00080000)

void tlb_init(void)
{
    uint32_t even_pfn = USEG_PADDR >> 12;    // Higher 20 bits.
    uint32_t odd_pfn = even_pfn + (PAGE_SIZE >> 12);

    C0_WRITE_PAGEMASK(TLB_PAGE_SIZE_256KB);
    C0_WRITE_ENTRYHI(USEG_VADDR);
    C0_WRITE_ENTRYLO0((even_pfn << 6) | TLB_WRITE_ENABLE | TLB_VALID | TLB_GLOBAL);
    C0_WRITE_ENTRYLO1((odd_pfn << 6) | TLB_WRITE_ENABLE | TLB_VALID | TLB_GLOBAL);
    // Write TLB entry.
    C0_TLBWI();
}
