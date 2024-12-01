/**
 * @file isviewer.c
 * @brief This module is a driver for writing to ISViewer.
 * 
 * ISViewer was a hardware development cartridge that could write messages to
 * a development machine. The real hardware is obscure today but the ISViewer
 * protocol was taken by emulator developers as target for debug logging.
 * 
 * We implement writing to ISViewer here to improve debugging on emulator.
 */

#include "intdef.h"
#include "dma.h"

// ISViewer register for magic value (to check ISViewer presence).
#define ISVIEWER_MAGIC_ADDR     0x13FF0000
// ISViewer register for circular buffer write pointer.
#define ISVIEWER_WRITE_ADDR     0x13FF0014
// ISViewer buffer.
#define ISVIEWER_BUFFER_ADDR    0x13FF0020
// ISViewer buffer length.
#define ISVIEWER_BUFFER_LEN     0x00000200

static bool is_enabled;

bool isviewer_init_impl(void)
{
    // To check whether an ISViewer is present (probably emulated),
    // write some data to the "magic" register. If we can read it
    // back, it means that there's some memory there and we can
    // hopefully use it.
    const uint32_t magic = 0x12345678;

    // Write inverted magic value to check if the memory is truly writable,
    // and to make sure there's no residual value that's equal to our magic value.
    io_write(ISVIEWER_MAGIC_ADDR, ~magic);
    if (io_read(ISVIEWER_MAGIC_ADDR) != ~magic) {
        return false;
    }

    io_write(ISVIEWER_MAGIC_ADDR, magic);
    return io_read(ISVIEWER_MAGIC_ADDR) == magic;
}

bool isviewer_init(void)
{
    is_enabled = isviewer_init_impl();
    return is_enabled;
}

void isviewer_write(const uint8_t* data, int len)
{
    if (!is_enabled)
    {
        return;
    }

    while (len > 0)
    {
        uint32_t l = (len < ISVIEWER_BUFFER_LEN) ? len : ISVIEWER_BUFFER_LEN;

        // Write 32-bit aligned words to copy the buffer. Notice that
        // we might overflow the input buffer if it's not a multiple
        // of 4 bytes but it doesn't matter because we are going to
        // write the exact number of bytes later.
        for (int i = 0; i < l; i += 4)
        {
            uint32_t value = ((uint32_t) data[0] << 24) |
                             ((uint32_t) data[1] << 16) |
                             ((uint32_t) data[2] <<  8) |
                             ((uint32_t) data[3] <<  0);
            io_write(ISVIEWER_BUFFER_ADDR + i, value);
            data += 4;
        }

        // Flush the data into the ISViewer.
        // We use write pointer register as length register,
        // but that's fine for emulators.
        io_write(ISVIEWER_WRITE_ADDR, l);
        len -= l;
    }
}

void printn(const char* data, int len)
{
    isviewer_write((uint8_t*) data, len);
}
