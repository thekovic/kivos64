#ifndef KIVOS64_CONTROLLER_H
#define KIVOS64_CONTROLLER_H

#include "intdef.h"

typedef union controller_buttons_u
{
    /** @brief Raw button data as a 16-bit value. */
    uint16_t raw;

    struct __attribute__((packed))
    {
        /** @brief State of the A button. */
        unsigned a : 1;
        /** @brief State of the B button. */
        unsigned b : 1;
        /** @brief State of the Z button. */
        unsigned z : 1;
        /** @brief State of the Start button. */
        unsigned start : 1;
        /** @brief State of the Dpad Up button. */
        unsigned d_up : 1;
        /** @brief State of the Dpad Down button. */
        unsigned d_down : 1;
        /** @brief State of the Dpad Left button. */
        unsigned d_left : 1;
        /** @brief State of the Dpad Right button. */
        unsigned d_right : 1;
        /** @brief Unused padding bits. */
        unsigned : 2;
        /** @brief State of the L button. */
        unsigned l : 1;
        /** @brief State of the R button. */
        unsigned r : 1;
        /** @brief State of the C-Up button. */
        unsigned c_up : 1;
        /** @brief State of the C-Down button. */
        unsigned c_down : 1;
        /** @brief State of the C-Left button. */
        unsigned c_left : 1;
        /** @brief State of the C-Right button. */
        unsigned c_right : 1;
    };
} controller_buttons_t; 

/** @brief Nintendo 64 controller data. */
typedef union controller_data_u
{
    /** @brief Raw input data as a 32-bit value. */
    uint32_t raw;

    struct __attribute__((packed))
    {
        /** @brief State of the buttons. */
        controller_buttons_t buttons;
        /** @brief State of the analog stick (X axis). */
        signed stick_x : 8;
        /** @brief State of the analog stick (Y axis). */
        signed stick_y : 8;
    };
} controller_data_t;

void controller_poll(void);

controller_data_t controller_get_inputs(void);

controller_buttons_t controller_get_buttons(void);

controller_buttons_t controller_get_buttons_pressed(void);

controller_buttons_t controller_get_buttons_released(void);

controller_buttons_t controller_get_buttons_held(void);

#endif
