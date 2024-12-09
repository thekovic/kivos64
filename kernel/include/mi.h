/**
 * @file mi.h
 * @brief The MIPS Interface (or MI) is one of multiple I/O interfaces in the RCP. It is the interface between the RCP and the VR4300 CPU, primarily used for enabling/disabling interrupts and checking their status. 
 */

#ifndef KIVOS64_MI_H
#define KIVOS64_MI_H

#include "intdef.h"

#define MI_REG_BASE                 (0xA4300000)

// Read bits.
#define MI_MODE_REPEAT_COUNT        (0b1111111)   // Repeat mode: number of times to repeat the value.
#define MI_MODE_REPEAT              (1 << 7)      // Repeat mode: fast repeated writes of fixed values.
#define MI_MODE_EBUS                (1 << 8)      // Access to E-Bus (9th bits of RDRAM).
#define MI_MODE_UPPER               (1 << 9)      // Upper mode (32-bit transfers are shifted in the upper half of the bus).
// Write bits.
#define MI_MODE_CLR_REPEAT          (1 << 7)      // Disable repeat mode.
#define MI_MODE_SET_REPEAT          (1 << 8)      // Enable repeat mode.
#define MI_MODE_CLR_EBUS            (1 << 9)      // Disable E-Bus access mode.
#define MI_MODE_SET_EBUS            (1 << 10)     // Enable E-Bus access mode.
#define MI_MODE_CLR_DPINT           (1 << 11)     // Clear DP interrupt.
#define MI_MODE_CLR_UPPER           (1 << 12)     // Disable upper mode.
#define MI_MODE_SET_UPPER           (1 << 13)     // Enable upper mode.

#define MI_INTERRUPT_SP             (1 << 1)      // Pending interrupt: RSP interrupt requested.
#define MI_INTERRUPT_SI             (1 << 2)      // Pending interrupt: SI transfer finished.
#define MI_INTERRUPT_AI             (1 << 3)      // Pending interrupt: AI buffer playback started.
#define MI_INTERRUPT_VI             (1 << 4)      // Pending interrupt: VI line match (normally, vblank).
#define MI_INTERRUPT_PI             (1 << 5)      // Pending interrupt: PI transfer finished.
#define MI_INTERRUPT_DP             (1 << 6)      // Pending interrupt: RDP SYNC_FULL has finished.

// Read bits.
#define MI_MASK_SP                  (1 << 0)          // Mask for RSP interrupt.
#define MI_MASK_SI                  (1 << 1)          // Mask for SI transfer interrupt.
#define MI_MASK_AI                  (1 << 2)          // Mask for AI buffer playback interrupt.
#define MI_MASK_VI                  (1 << 3)          // Mask for VI line match interrupt.
#define MI_MASK_PI                  (1 << 4)          // Mask for PI transfer interrupt.
#define MI_MASK_DP                  (1 << 5)          // Mask for RDP SYNC_FULL interrupt.
// Write bits.
#define MI_MASK_CLR_SP              (1 << 0)       // Disable RSP interrupt.
#define MI_MASK_SET_SP              (1 << 1)       // Enable RSP interrupt.
#define MI_MASK_CLR_SI              (1 << 2)       // Disable SI transfer interrupt.
#define MI_MASK_SET_SI              (1 << 3)       // Enable SI transfer interrupt.
#define MI_MASK_CLR_AI              (1 << 4)       // Disable AI buffer playback interrupt.
#define MI_MASK_SET_AI              (1 << 5)       // Enable AI buffer playback interrupt.
#define MI_MASK_CLR_VI              (1 << 6)       // Disable VI line match interrupt.
#define MI_MASK_SET_VI              (1 << 7)       // Enable VI line match interrupt.
#define MI_MASK_CLR_PI              (1 << 8)       // Disable PI transfer interrupt.
#define MI_MASK_SET_PI              (1 << 9)       // Enable PI transfer interrupt.
#define MI_MASK_CLR_DP              (1 << 10)      // Disable RDP SYNC_FULL interrupt.
#define MI_MASK_SET_DP              (1 << 11)      // Enable RDP SYNC_FULL interrupt.

typedef struct MI_registers_s
{
    uint32_t mode;              // MI_MODE register.
    uint32_t version;           // MI_VERSION register.
    uint32_t interrupt;         // MI_INTERRUPT register.
    uint32_t mask;              // MI_MASK register.
} MI_registers_t;

extern volatile MI_registers_t* const MI_regs;

#endif
