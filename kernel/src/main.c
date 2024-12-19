#include "intdef.h"
#include "userspace.h"
#include "system.h"

#include "game/sprite_astroman.h"
#include "game/sprite_k.h"
#include "game/sprite_i.h"
#include "game/sprite_v.h"

#define RES_WIDTH  (320)
#define RES_HEIGHT (240)

#define GAME_SPACE_MARGIN   (2)
#define SPAWN_MARGIN        (16)
#define SPAWN_AREA_X        (RES_WIDTH - 3 * SPAWN_MARGIN)
#define SPAWN_AREA_Y        (RES_HEIGHT - 3 * SPAWN_MARGIN)

static int frame_counter = 0;
static int collected_letters = 0;

typedef struct game_obj_s
{
    int x;
    int y;
    bool spawned;
    surface_t sprite;
} game_obj_t;

game_obj_t player = (game_obj_t) {
    .x = 160,
    .y = 120,
    .spawned = true,
    .sprite = (surface_t) {
        .width = 16,
        .height = 32,
        .buffer = (uint32_t*) sprite_astroman_data
    }
};

game_obj_t letter_k = (game_obj_t) {
    .x = 0,
    .y = 0,
    .spawned = false,
    .sprite = (surface_t) {
        .width = 32,
        .height = 32,
        .buffer = (uint32_t*) sprite_k_data
    }
};

game_obj_t letter_i = (game_obj_t) {
    .x = 0,
    .y = 0,
    .spawned = false,
    .sprite = (surface_t) {
        .width = 16,
        .height = 32,
        .buffer = (uint32_t*) sprite_i_data
    }
};

game_obj_t letter_v = (game_obj_t) {
    .x = 0,
    .y = 0,
    .spawned = false,
    .sprite = (surface_t) {
        .width = 32,
        .height = 32,
        .buffer = (uint32_t*) sprite_v_data
    }
};

#define GAME_OBJ_COUNT  (4)
game_obj_t* game_objs[GAME_OBJ_COUNT] = {&player, &letter_k, &letter_i, &letter_v};

bool game_obj_check_if_collides(game_obj_t* game_obj1, game_obj_t* game_obj2)
{
    if (!game_obj2->spawned)
    {
        return false;
    }

    int x1_min = game_obj1->x;
    int x1_max = game_obj1->x + game_obj1->sprite.width;
    int y1_min = game_obj1->y;
    int y1_max = game_obj1->y + game_obj1->sprite.height;
    
    int x2_min = game_obj2->x;
    int x2_max = game_obj2->x + game_obj2->sprite.width;
    int y2_min = game_obj2->y;
    int y2_max = game_obj2->y + game_obj2->sprite.height;

    bool overlap_x = (x1_max >= x2_min) && (x2_max >= x1_min);
    bool overlap_y = (y1_max >= y2_min) && (y2_max >= y1_min);

    return (overlap_x && overlap_y);
}

void game_obj_snap_inbounds(game_obj_t* game_obj)
{
    if (game_obj->x < GAME_SPACE_MARGIN)
    {
        game_obj->x = GAME_SPACE_MARGIN;
    }
    if (game_obj->x + game_obj->sprite.width > (RES_WIDTH - GAME_SPACE_MARGIN))
    {
        game_obj->x = RES_WIDTH - GAME_SPACE_MARGIN - game_obj->sprite.width;
    }
    if (game_obj->y < GAME_SPACE_MARGIN)
    {
        game_obj->y = GAME_SPACE_MARGIN;
    }
    if (game_obj->y + game_obj->sprite.height > (RES_HEIGHT - GAME_SPACE_MARGIN))
    {
        game_obj->y = RES_HEIGHT - GAME_SPACE_MARGIN - game_obj->sprite.height;
    }
}

void game_obj_draw(surface_t* display, game_obj_t* game_obj)
{
    if (!game_obj->spawned)
    {
        return;
    }

    graphics_draw_surface_alpha_user(display, game_obj->x, game_obj->y, &game_obj->sprite);
}

void game_obj_spawn(game_obj_t* game_obj)
{
    if (game_obj->spawned)
    {
        return;
    }
    // Spawn at random location.
    game_obj->x = (((frame_counter + player.y) * (collected_letters + 1)) % SPAWN_AREA_X) + SPAWN_MARGIN;
    game_obj->y = (((frame_counter + player.x) * (collected_letters + 1)) % SPAWN_AREA_Y) + SPAWN_MARGIN;
    // Check if chosen location doesn't collide with another object.
    for (int i = 0; i < GAME_OBJ_COUNT; i++)
    {
        game_obj_t* other_obj = game_objs[i];
        // Don't check against itself.
        if (game_obj == other_obj)
        {
            continue;
        }

        if (game_obj_check_if_collides(game_obj, other_obj))
        {
            return;
        }
    }

    game_obj->spawned = true;
}

void main(void)
{
    display_init_user(RES_WIDTH, RES_HEIGHT, FILTER_NONE);

    uint32_t bg_fill = RGBA32(0, 0, 0, 255);

    for (;;)
    {
        controller_buttons_t buttons = controller_poll_and_get_buttons_held_user();
        if (buttons.d_up || buttons.c_up)
        {
            player.y--;
        }
        if (buttons.d_down || buttons.c_down)
        {
            player.y++;
        }
        if (buttons.d_left || buttons.c_left)
        {
            player.x--;
        }
        if (buttons.d_right || buttons.c_right)
        {
            player.x++;
        }

        // Keep player in bounds.
        game_obj_snap_inbounds(&player);

        if (frame_counter % 120 == 40)
        {
            game_obj_spawn(&letter_k);
        }
        if (frame_counter % 120 == 80)
        {
            game_obj_spawn(&letter_i);
        }
        if (frame_counter % 120 == 119)
        {
            game_obj_spawn(&letter_v);
        }

        for (int i = 1; i < GAME_OBJ_COUNT; i++)
        {
            game_obj_t* letter = game_objs[i];
            if (!game_obj_check_if_collides(&player, letter))
            {
                continue;
            }

            letter->spawned = false;
            // Increment series if in right order.
            if (i == (collected_letters + 1))
            {
                collected_letters++;
            }
            else
            {
                collected_letters = 0;
            }
            // Play sound.
            if (collected_letters == 3)
            {
                audio_play_square_wave_user(220.0f, 10.0f / 60.0f, 0.2f);
                collected_letters = 0;
            }
            else
            {
                audio_play_square_wave_user(110.0f, 5.0f / 60.0f, 0.2f);
            }
        }
        
        surface_t* display = display_get_user();
        graphics_fill_user(display, bg_fill);
        for (int i = 0; i < GAME_OBJ_COUNT; i++)
        {
            game_obj_draw(display, game_objs[i]);
        }
        display_show_user(display);

        frame_counter++;
    }
}
