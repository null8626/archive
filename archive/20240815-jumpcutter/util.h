#ifndef _JUMPCUTTER_UTIL_H
#define _JUMPCUTTER_UTIL_H

#include "jumpcutter.h"
#include <float.h>

typedef struct {
  const char* message;
} jumpcutter_error_t;

void jumpcutter_packet_data_apply(jumpcutter_packet_data_t* d, AVPacket* packet);
double jumpcutter_packet_data_get_timestamp(jumpcutter_packet_data_t* d);
void jumpcutter_initiate_output_stream_info(const AVPacket* packet, jumpcutter_output_stream_info_t* output, jumpcutter_input_stream_info_t* input);

bool jumpcutter_silent_portions_new(jumpcutter_silent_portions_t* portions, jumpcutter_error_t* error);
bool jumpcutter_silent_portions_push(jumpcutter_silent_portions_t* portions, const jumpcutter_silent_portion_t* portion, jumpcutter_error_t* error);
void jumpcutter_silent_portions_free(jumpcutter_silent_portions_t* portions);
bool jumpcutter_silent_portion_new(jumpcutter_silent_portion_t* portion, const double start, jumpcutter_error_t* error);

inline void jumpcutter_current_silent_portion_new(jumpcutter_current_silent_portion_t* current_portion) {
  current_portion->previously_is_silent = 2;
  jumpcutter_silent_portion_new(&current_portion->portion, DBL_MAX, NULL);
}

bool jumpcutter_timestamps_new(jumpcutter_timestamps_t* timestamps, jumpcutter_error_t* error);
bool jumpcutter_timestamps_push(jumpcutter_timestamps_t* timestamps, const double timestamp, jumpcutter_error_t* error);

inline void jumpcutter_timestamps_clear(jumpcutter_timestamps_t* timestamps) {
  timestamps->length = 0;
}

void jumpcutter_timestamps_free(jumpcutter_timestamps_t* timestamps);

inline void jumpcutter_silent_portion_free(jumpcutter_silent_portion_t* portion) {
  jumpcutter_timestamps_free(&portion->audio_packets);
  jumpcutter_timestamps_free(&portion->video_packets);
}

#endif