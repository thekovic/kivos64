#ifndef KIVOS64_AI_H
#define KIVOS64_AI_H

#include "intdef.h"

#define AI_REG_BASE                 (0xA4500000)

/** @brief Bit that starts AI DMA. */
#define AI_CONTROL_DMA_START        (1)
/** @brief Bit representing that the AI is busy. */
#define AI_STATUS_BUSY              (1 << 30)
/** @brief Bit representing that the AI is full. */
#define AI_STATUS_FULL              (1 << 31)
#define AI_DACRATE_NTSC             (48681818)

typedef struct AI_registers_s
{
    /** @brief Pointer to uncached memory buffer of samples to play. */
    uint32_t address;
    /**
     * @brief Size of the buffer to be played in bytes. Should be number of samples * 2 (stereo) * 2 (uint16_t).
     * Aligned to 8 bytes.
     */
    uint32_t length;
    /** @brief DMA start register. */
    uint32_t control;
    /** @brief AI status register.  Writing to this register acknowledges AI interrupt. */
    uint32_t status;
    /** @brief Rate at which the buffer should be played. */
    uint32_t dacrate;
    /** @brief Half-rate at which each single bit of a sample is shifted into the DAC. */
    uint32_t bitrate;
} AI_registers_t;

extern volatile AI_registers_t* const AI_regs;

#endif
