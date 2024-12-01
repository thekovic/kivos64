/**
 * @file cop1.h
 * @brief N64 COP1 Interface
 * 
 * The coprocessor 1 (COP1) is implemented as a floating point unit (FPU).
 */

#ifndef KIVOS64_COP1_H
#define KIVOS64_COP1_H

// COP1 Control/Status bits definitions (VR4300 manual page 211).
#define C1_FLAG_INEXACT_OP          (1 << 2)         // Flag recording inexact operation.
#define C1_FLAG_UNDERFLOW           (1 << 3)         // Flag recording underflow.
#define C1_FLAG_OVERFLOW            (1 << 4)         // Flag recording overflow.
#define C1_FLAG_DIV_BY_0            (1 << 5)         // Flag recording division by zero.
#define C1_FLAG_INVALID_OP          (1 << 6)         // Flag recording invalid operation.

#define C1_ENABLE_INEXACT_OP        (1 << 7)         // Enable inexact operation exception.
#define C1_ENABLE_UNDERFLOW         (1 << 8)         // Enable underflow exception.
#define C1_ENABLE_OVERFLOW          (1 << 9)         // Enable overflow exception.
#define C1_ENABLE_DIV_BY_0          (1 << 10)        // Enable division by zero exception.
#define C1_ENABLE_INVALID_OP        (1 << 11)        // Enable invalid operation exception.
#define C1_ENABLE_MASK              (0b11111 << 7)   // Mask for all enable bits.

#define C1_CAUSE_INEXACT_OP         (1 << 12)         // Triggered inexact operation exception.
#define C1_CAUSE_UNDERFLOW          (1 << 13)         // Triggered underflow exception.
#define C1_CAUSE_OVERFLOW           (1 << 14)         // Triggered overflow exception.
#define C1_CAUSE_DIV_BY_0           (1 << 15)         // Triggered division by zero exception.
#define C1_CAUSE_INVALID_OP         (1 << 16)         // Triggered invalid operation exception.
#define C1_CAUSE_NOT_IMPLEMENTED    (1 << 17)         // Triggered not implemented exception.
#define C1_CAUSE_MASK               (0b111111 << 12)  // Mask for all cause bits.

#define C1_STATUS_COMPARE           (1 << 23)         // Result of a floating point compare operation.
#define C1_STATUS_FS                (1 << 24)         // Flush denormals instead of causing exception (page 213).

/** @brief Read the COP1 floating-point control register 31.
 *
 * FCR31 is also known as the Control/Status register. It keeps control and
 * status data for the FPU.
 */
#define C1_FCR31() ({ \
    uint32_t x; \
    asm volatile("cfc1 %0,$f31":"=r"(x)); \
    x; \
})

/** @brief Write the COP1 floating-point control register 31. */
#define C1_WRITE_FCR31(x) ({ \
    asm volatile("ctc1 %0,$f31"::"r"(x)); \
})

#endif
