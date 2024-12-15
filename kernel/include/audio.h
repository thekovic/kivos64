#ifndef KIVOS64_AUDIO_H
#define KIVOS64_AUDIO_H

#include "intdef.h"

int16_t* audio_get_buffer(void);

void audio_play_buffer(int16_t* audio_buffer);

int audio_play_square_wave(float frequency, float duration, float volume);

#endif
