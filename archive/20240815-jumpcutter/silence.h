#ifndef _JUMPCUTTER_SILENCE_H
#define _JUMPCUTTER_SILENCE_H

#include "jumpcutter.h"

bool jumpcutter_skip_frame(jumpcutter_silent_portions_t* portions, size_t* index, const double timestamp, const uint8_t frame_margin);
int jumpcutter_process_initial_audio_stream(jumpcutter_t* j, const float silent_threshold, jumpcutter_error_t* error);
int jumpcutter_process_initial_video_stream(jumpcutter_t* j, jumpcutter_error_t* error);
bool jumpcutter_flush_after_initial_read(jumpcutter_t* j, jumpcutter_error_t* error);

#endif