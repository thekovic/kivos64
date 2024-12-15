#include "ai.h"
#include "interrupt.h"
#include "memory.h"
#include "system.h"

/** @brief Maximum number of audio buffers. */
#define NUM_BUFFERS         (4)
#define SAMPLES_PER_SECOND  (16)
#define MAX_AMPLITUDE       (32767)

/** @brief The actual frequency the AI will run at. */
static int __sample_rate = 0;
/** @brief Array of pointers to the allocated buffers. */
static int16_t* __buffers[NUM_BUFFERS];
/** @brief The buffer size in stereo samples for each buffer allocated. */
static int __buffer_size = 0;

/** @brief Index of the current playing buffer. */
static int __now_playing = 0;
/** @brief Bitmask of buffers that are acquired to be filled with samples. */
static uint32_t __acquired_mask = 0;
/** @brief Bitmask of buffers that are pending to be played. */
static uint32_t __pending_mask = 0;

/**
 * @brief Send next available chunks of audio data to the AI.
 *
 * This function is called whenever internal buffers are running low.
 * It will send as many buffers as possible to the AI until the AI is full.
 */
void __audio_callback()
{
    interrupt_disable();

    uint32_t status = AI_regs->status;
    // If AI is not full, schedule another DMA, if pending.
    while (!(status & AI_STATUS_FULL))
    {
        int next = (__now_playing + 1) % NUM_BUFFERS;
        // If next isn't pending to be played, get out.
        if (!(__pending_mask & (1 << next)))
        {
            break;
        }

        // Enqueue next buffer.
        AI_regs->address = (uint32_t) __buffers[next];
        AI_regs->length = ((2 * __buffer_size * sizeof(int16_t)) & ~0x7);

        // Start DMA.
        AI_regs->control = 1;

        __now_playing = next;
        __pending_mask &= ~(1 << next);

        status = AI_regs->status;
    }

    interrupt_enable();
}

void audio_init(int frequency)
{
    // Calculate DAC dacrate. This is based on the VI clock rate (as the VI clock
    // is also used for the AI output), divided by the requested frequency, rounding up.
    int dacrate = ((2 * AI_DACRATE_NTSC / frequency) + 1) / 2;
    // Docs say to use this.
    int bitrate = dacrate / 66;
    // Cap it at 16 (hardware limit).
    if (bitrate > 16)
    {
        bitrate = 16;
    }

    // Write setup to registers (minus 1 because that's how the registers work apparently).
    AI_regs->dacrate = dacrate - 1;
    AI_regs->bitrate = bitrate - 1;

    __sample_rate = frequency;
    __buffer_size = ((__sample_rate / SAMPLES_PER_SECOND) & ~0x7);

    for(int i = 0; i < NUM_BUFFERS; i++)
    {
        // 16-bit stereo buffers, interleaved, plus 8 bytes of padding.
        __buffers[i] = malloc_uncached((2 * __buffer_size * sizeof(int16_t)) + 8);

        // Workaround AI DMA hardware bug. If a buffer ends exactly
        // at a 0x2000 address boundary, AI DMA gets confused because
        // of a delayed internal carry. Since we allocated 8 bytes of padding,
        // we can move our pointer a bit.
        if (((uint32_t)(__buffers[i] + 2 * __buffer_size) & 0x1FFF) == 0)
        {
            __buffers[i] += 4;
        }
        
        memset(__buffers[i], 0, 2 * __buffer_size * sizeof(int16_t));
    }

    __now_playing = 0;
    __acquired_mask = 0;
    __pending_mask = 0;

    // Set up hardware to notify us when it needs more data.
    interrupt_set_AI(true);
}

int16_t* audio_get_buffer(void) 
{
    interrupt_disable();

    int next = (__now_playing + 1) % NUM_BUFFERS;
    // If next buffer is full, try to empty it out asap.
    while (__pending_mask & (1 << next))
    {
        __audio_callback();
        // Give brief window to other interrupts while we're looping.
        interrupt_enable();
        interrupt_disable();
    }

    __acquired_mask |= 1 << next;
    interrupt_enable();

    return __buffers[next];
}

void audio_play_buffer(int16_t* audio_buffer)
{
    interrupt_disable();

    int i = 0;
    while (i <= NUM_BUFFERS)
    {
        if (audio_buffer == __buffers[i])
        {
            break;
        }
        i++;
    }

    assert(i >= 0 && i < NUM_BUFFERS, "audio_play_buffer: Buffer address is not valid!");
    assert(!(__pending_mask & (1 << i)), "audio_play_buffer: Called on the same buffer twice.");
    assert(__acquired_mask & (1 << i), "audio_play_buffer: Called on a buffer not acquired first.");

    __acquired_mask &= ~(1 << i);
    __pending_mask |= (1 << i);
    // Go into callback to kickstart DMA if nothing is currently happening.
    __audio_callback();

    interrupt_enable();
}

int audio_play_square_wave(float frequency, float duration, float volume)
{
    // Total number of mono samples
    int num_samples = (int) (__sample_rate * duration);
    int samples_per_cycle = (int) (__sample_rate / frequency);
    int half_cycle_samples = samples_per_cycle / 2;
    int volume_real = (int) (volume * MAX_AMPLITUDE);

    int16_t* buffer = NULL;
    int buffer_size = 0;
    int samples_written = 0;
    int i = 0;

    while (num_samples > 0)
    {
        if (!buffer)
        {
            buffer = audio_get_buffer();
            buffer_size = __buffer_size;
            i = 0;
        }

        // Determine if the current sample is in the positive or negative half-cycle.
        short sample_value = (samples_written % samples_per_cycle < half_cycle_samples) ? (volume_real) : -(volume_real);

        // Write the sample value to both left and right channels (stereo).
        buffer[2 * i] = sample_value;       // Left channel
        buffer[2 * i + 1] = sample_value;   // Right channel

        num_samples--;
        samples_written++;

        i++;
        buffer_size--;

        if (buffer_size <= 0)
        {
            audio_play_buffer(buffer);
            buffer = NULL;
        }
    }

    return samples_written;
}
