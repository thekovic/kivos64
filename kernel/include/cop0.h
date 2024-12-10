#ifndef KIVOS64_COP0_H
#define KIVOS64_COP0_H

/**
 * @brief Read the COP0 Index register.
 * 
 * This register is used during TLB programming. It holds the index of the TLB
 * entry on 5 bits (0-31).
 */
#define C0_INDEX() ({ \
    uint32_t x; \
    asm volatile("mfc0 %0,$0":"=r"(x)); \
    x; \
})
/**
 * @brief Write the COP0 Index register.
 * 
 * This register is used during TLB programming. It holds the index of the TLB
 * entry on 5 bits (0-31).
 */
#define C0_WRITE_INDEX(x)    asm volatile("mtc0 %0,$0; nop; nop"::"r"(x))


/**
 * @brief Read the COP0 Random register.
 * 
 * This register is used during TLB programming. It holds the index of a random
 * TLB entry on 5 bits (0-31).
 */
#define C0_RANDOM() ({ \
    uint32_t x; \
    asm volatile("mfc0 %0,$1":"=r"(x)); \
    x; \
})


/**
 * @brief Read the COP0 EntryLo0 register.
 * 
 * This register is used during TLB programming. It holds the configuration
 * of the physical memory entry (even bank) for the TLB slot being accessed.
 */
#define C0_ENTRYLO0() ({ \
    uint32_t x; \
    asm volatile("mfc0 %0,$2":"=r"(x)); \
    x; \
})
/**
 * @brief Write the COP0 EntryLo0 register.
 * 
 * This register is used during TLB programming. It holds the configuration
 * of the physical memory entry (even bank) for the TLB slot being accessed.
 */
#define C0_WRITE_ENTRYLO0(x) asm volatile("mtc0 %0,$2; nop; nop"::"r"(x))


/**
 * @brief Read the COP0 EntryLo1 register.
 * 
 * This register is used during TLB programming. It holds the configuration
 * of the physical memory entry (odd bank) for the TLB slot being accessed.
 */
#define C0_ENTRYLO1() ({ \
    uint32_t x; \
    asm volatile("mfc0 %0,$3":"=r"(x)); \
    x; \
})
/**
 * @brief Write the COP0 EntryLo1 register.
 * 
 * This register is used during TLB programming. It holds the configuration
 * of the physical memory entry (odd bank) for the TLB slot being accessed.
 */
#define C0_WRITE_ENTRYLO1(x) asm volatile("mtc0 %0,$3; nop; nop"::"r"(x))


/**
 * @brief Read the COP0 Context register.
 * 
 * This register contains the pointer to an entry in the page table entry (PTE) array on memory.
 */
#define C0_CONTEXT() ({ \
    uint64_t x; \
    asm volatile("dmfc0 %0,$4":"=r"(x)); \
    x; \
})
/**
 * @brief Write the COP0 EntryLo1 register.
 * 
 * This register contains the pointer to an entry in the page table entry (PTE) array on memory.
 */
#define C0_WRITE_CONTEXT(x) asm volatile("dmtc0 %0,$4; nop; nop"::"r"(x))


/**
 * @brief Read the COP0 PageMask register.
 * 
 * This register is used during TLB programming. It holds the bitmask that
 * configures the page size of the TLB slot being accessed.
 */
#define C0_PAGEMASK() ({ \
    uint32_t x; \
    asm volatile("mfc0 %0,$5":"=r"(x)); \
    x; \
})
/**
 * @brief Write the COP0 PAGEMASK register.
 * 
 * This register is used during TLB programming. It holds the bitmask that
 * configures the page size of the TLB slot being accessed.
 */
#define C0_WRITE_PAGEMASK(x) asm volatile("mtc0 %0,$5; nop; nop"::"r"(x))


/**
 * @brief Read the COP0 WIRED register.
 * 
 * This register is used during TLB programming. It allows to partition TLB
 * slots between fixed slots and random slots. The fixed slot pool is the
 * range [0..WIRED] and the random pool is the range [WIRED..32]
 */
#define C0_WIRED() ({ \
    uint32_t x; \
    asm volatile("mfc0 %0,$6":"=r"(x)); \
    x; \
})
/**
 * @brief Write the COP0 WIRED register.
 * 
 * This register is used during TLB programming. It allows to partition TLB
 * slots between fixed slots and random slots. The fixed slot pool is the
 * range [0..WIRED] and the random pool is the range [WIRED..32]
 */
#define C0_WRITE_WIRED(x) asm volatile("mtc0 %0,$6; nop; nop"::"r"(x))


/**
 * @brief Read the COP0 BadVAddr register.
 *
 * This register is a read-only register holding the last virtual address to be translated which became
 * invalid, or a virtual address for which an addressing error occurred.
 */
#define C0_BADVADDR() ({ \
	uint64_t x; \
	asm volatile("dmfc0 %0,$8" : "=r" (x) : ); \
	x; \
})


/** 
 * @brief Read the COP0 Count register.
 * 
 * This register acts as a timer, incrementing at a constant rate
 * (half the PClock speed) whether or not instructions are being executed.
 */
#define C0_COUNT() ({ \
    uint32_t x; \
    asm volatile("mfc0 %0,$9":"=r"(x)); \
    x; \
})
/** 
 * @brief Write the COP0 Count register.
 * 
 * This register acts as a timer, incrementing at a constant rate
 * (half the PClock speed) whether or not instructions are being executed.
 */
#define C0_WRITE_COUNT(x) ({ \
    asm volatile("mtc0 %0,$9"::"r"(x)); \
})


/**
 * @brief Read the COP0 EntryHi register.
 * 
 * This register is used during TLB programming. It holds the configuration
 * of the virtual memory entry for the TLB slot being accessed.
 */
#define C0_ENTRYHI() ({ \
    uint32_t x; \
    asm volatile("mfc0 %0,$10":"=r"(x)); \
    x; \
})
/**
 * @brief Write the COP0 EntryHi register.
 * 
 * This register is used during TLB programming. It holds the configuration
 * of the virtual memory entry for the TLB slot being accessed.
 */
#define C0_WRITE_ENTRYHI(x)  asm volatile("mtc0 %0,$10; nop; nop"::"r"(x))


/** @brief Read the COP0 Compare register. */
#define C0_COMPARE() ({ \
    uint32_t x; \
    asm volatile("mfc0 %0,$11":"=r"(x)); \
    x; \
})
/**
 * @brief Write the COP0 Compare register.
 *
 * This register is used to generate a timer interrupt; it maintains a stable 
 * value that does not change on its own. When the value of the Compare register
 * equals the value of the Count register, bit 7 (interrupt) in the Cause register
 * is set. Writing a value to the Compare register, as a side effect, clears the timer interrupt. 
 */
#define C0_WRITE_COMPARE(x) ({ \
    asm volatile("mtc0 %0,$11"::"r"(x)); \
})

#define C0_STATUS_IE        (1 << 0)    // Status: Global interrupt enable (0=off, 1=on).
#define C0_STATUS_EXL       (1 << 1)    // Status: Exception level (0=normal, 1=exception).
#define C0_STATUS_ERL       (1 << 2)    // Status: Error level (0=normal, 1=error).
#define C0_KSU_KERNEL       (0x0 << 3)    // Signifies Kernel mode for the COP0 Status register.
#define C0_KSU_SUPERVISOR   (0x1 << 3)    // Signifies Supervisor mode for the COP0 Status register.
#define C0_KSU_USER         (0x2 << 3)    // Signifies User mode for the COP0 Status register.
#define C0_STATUS_KSU       (0x3 << 3)    // Status: Mode bits.
#define C0_STATUS_UX        (1 << 5)    // Status: 64-bit addressing in User mode (0=off, 1=on).
#define C0_STATUS_SX        (1 << 6)    // Status: 64-bit addressing in Supervisor mode (0=off, 1=on).
#define C0_STATUS_KX        (1 << 7)    // Status: 64-bit addressing in Kernel mode (0=off, 1=on).
#define C0_INTERRUPT_0      (1 << 8)    // Status/Cause: SW interrupt 0
#define C0_INTERRUPT_1      (1 << 9)    // Status/Cause: SW interrupt 1
#define C0_INTERRUPT_RCP    (1 << 10)   // Status/Cause: HW interrupt 2 (RCP)
#define C0_INTERRUPT_CART   (1 << 11)   // Status/Cause: HW interrupt 3 (CART)
#define C0_INTERRUPT_RESET  (1 << 12)   // Status/Cause: HW interrupt 4 (RESET)
#define C0_INTERRUPT_5      (1 << 13)   // Status/Cause: HW interrupt 5
#define C0_INTERRUPT_6      (1 << 14)   // Status/Cause: HW interrupt 6
#define C0_INTERRUPT_TIMER  (1 << 15)   // Status/Cause: HW interrupt 7 (Timer)
#define C0_INTERRUPT_MASK   (0xFF80)    // Mask for bits 15 to 7.
#define C0_STATUS_SR        (1 << 20)   // Status: Indicates soft reset has occured.
#define C0_STATUS_FR        (1 << 26)   // Status: Enable additional FP registers (0=16 registers, 1=32 registers).
#define C0_STATUS_RP        (1 << 27)   // Status: Enable low-power mode (0=off, 1=on).
#define C0_STATUS_CU0       (1 << 28)   // Status: Enable use of COP0 (ignored in Kernel mode).
#define C0_STATUS_CU1       (1 << 29)   // Status: Enable use of COP1.
/** @brief Read the COP0 Status register. */
#define C0_STATUS() ({ \
    uint32_t x; \
    asm volatile("mfc0 %0,$12":"=r"(x)); \
    x; \
})
/** @brief Write the COP0 Status register. */
#define C0_WRITE_STATUS(x) ({ \
    asm volatile("mtc0 %0,$12"::"r"(x)); \
})


#define C0_CAUSE_EXC                (0b11111 << 2)    // Cause: Last exception code (5 bits).
#define C0_CAUSE_EXC_SYSCALL        (8 << 2)
#define C0_CAUSE_EXC_BREAKPOINT     (9 << 2)
#define C0_CAUSE_EXC_COP            (11 << 2)
#define C0_CAUSE_EXC_FP             (15 << 2)
/**
 * @brief Get the exception code value from the COP0 status register value.
 */
#define C0_GET_CAUSE_EXC(value)     (((value) & C0_CAUSE_EXC_CODE) >> 2)
#define C0_CAUSE_CE                 (1 << 29 || 1 << 28)   // Cause: Number of the coprocessor which caused a Coprocessor Unusable exception.
/**
 * @brief Get the CE value from the COP0 status register.
 *
 * Gets the Coprocessor unit number referenced by a coprocessor unusable
 * exception from the given COP0 Status register value.
 */
#define C0_GET_CAUSE_CE(value)      (((reg_value) & C0_CAUSE_CE) >> 28)
#define C0_CAUSE_BD                 (1 << 31)   // Cause: Indicates whether the last exception occurred has been executed in a branch delay slot. (0=normal, 1=delay slot).
/**
 * @brief Read the COP0 Cause register.
 */
#define C0_CAUSE() ({ \
	uint32_t x; \
	asm volatile("mfc0 %0,$13" : "=r" (x) : ); \
	x; \
})
/**
 * @brief Write the COP0 Cause register.
 */
#define C0_WRITE_CAUSE(x) ({ \
    asm volatile("mtc0 %0,$13"::"r"(x)); \
})


/**
 * @brief Read the COP0 EPC register.
 *
 * This register is the return from exception program counter. For asynchronous
 * exceptions it points to the place to continue execution whereas for synchronous
 * (caused by code) exceptions, point to the instruction causing the fault condition,
 * which needs correction in the exception handler. This macro is for reading its value.
 */
#define C0_EPC() ({ \
	uint32_t x; \
	asm volatile("mfc0 %0,$14" : "=r" (x) : ); \
	x; \
})


/**
 * @brief Read the COP0 WatchLo register
 * 
 * This register is used during watchpoint programming. It allows to trigger
 * an exception when a memory access occurs on a specific memory location.
 */
#define C0_WATCHLO() ({ \
    uint32_t x; \
    asm volatile("mfc0 %0,$18":"=r"(x)); \
    x; \
})
/**
 * @brief Write the COP0 WatchLo register
 * 
 * This register is used during watchpoint programming. It allows to trigger
 * an exception when a memory access occurs on a specific memory location.
 */
#define C0_WRITE_WATCHLO(x) asm volatile("mtc0 %0,$18"::"r"(x))


/* Flag bits valid for COP0 EntryLo0/EntryLo1 registers */
#define C0_ENTRYLO_GLOBAL      (1 << 0)       // EntryLo: mapping is global (all ASIDs)
#define C0_ENTRYLO_VALID       (1 << 1)       // EntryLo: mapping is active (not disabled)
#define C0_ENTRYLO_DIRTY       (1 << 2)       // EntryLo: mapping is writable

/* Flag bits valid for COP0 Index register */
#define C0_INDEX_PROBE_FAILED  (1 << 31)      // Index: set when a TLBP probe failed to find a match


/**
 * @brief Translation Lookaside Buffer Write Index instruction.
 * 
 * This opcode is used during TLB programming. It writes the TLB slot referenced
 * by INDEX with the contents of PAGEMASK, ENTRYHI, ENTRYLO0, ENTRYLO1.
 */
#define C0_TLBWI()           asm volatile("tlbwi; nop; nop; nop; nop")

/**
 * @brief Translation Lookaside Buffer Write Random instruction.
 * 
 * This opcode is used during TLB programming. It writes a random TLB slot with
 * the contents of PAGEMASK, ENTRYHI, ENTRYLO0, ENTRYLO1. THe slot is selected
 * in the random pool (slots in the range from WIRED to 31).
 */
#define C0_TLBWR()           asm volatile("tlbwr; nop; nop; nop; nop")

/**
 * @brief Translation Lookaside Buffer Read instruction.
 * 
 * This opcode is used during TLB programming. It reads the contents of the TLB
 * slot referenced by INDEX into the registers PAGEMASK, ENTRYHI, ENTRYLO0 and
 * ENTRYLO1.
 */
#define C0_TLBR()            asm volatile("tlbr; nop; nop; nop; nop")

/**
 * @brief Translation Lookaside Buffer Probe instruction.
 * 
 * This opcode is used during TLB programming. It probes the current TLB slots
 * using ENTRYHI (virtual address) to find a matching slot. If it finds, it
 * loads its index into INDEX. Otherwise, it sets the C0_INDEX_PROBE_FAILED bit
 * in INDEX.
 */
#define C0_TLBP()            asm volatile("tlbp; nop; nop; nop; nop")

#endif
