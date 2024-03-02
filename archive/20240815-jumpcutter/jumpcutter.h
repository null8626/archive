#ifndef _JUMPCUTTER_H
#define _JUMPCUTTER_H

#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(error: 4020)
#pragma warning(error: 4024)
#pragma warning(error: 4028)
#pragma warning(error: 4029)
#pragma warning(error: 4047)
#pragma warning(error: 4098)
#pragma warning(error: 4133)
#endif

typedef struct {
  int stream_id;
  AVStream* stream;
} jumpcutter_input_stream_info_t;

typedef struct {
  AVFormatContext* context;
  jumpcutter_input_stream_info_t audio;
  jumpcutter_input_stream_info_t video;
  AVCodecContext* audio_codec_context;
  AVFrame* audio_frame;
} jumpcutter_input_t;

typedef struct {
  double* timestamps;
  size_t length;
  size_t capacity;
} jumpcutter_timestamps_t;

typedef struct {
  double start;
  double end;
  jumpcutter_timestamps_t audio_packets;
  jumpcutter_timestamps_t video_packets;
} jumpcutter_silent_portion_t;

typedef struct {
  jumpcutter_silent_portion_t* portions;
  size_t length;
  size_t capacity;
} jumpcutter_silent_portions_t;

typedef struct {
  jumpcutter_silent_portion_t portion;
  uint8_t previously_is_silent;
} jumpcutter_current_silent_portion_t;

typedef struct {
  double time_base;
  int64_t pts;
  int64_t dts;
  int64_t duration;
  size_t index;
} jumpcutter_packet_data_t;

typedef struct {
  AVStream* stream;
  int stream_id;
  jumpcutter_packet_data_t packet_data;
} jumpcutter_output_stream_info_t;

typedef struct {
  AVFormatContext* context;
  jumpcutter_output_stream_info_t audio;
  jumpcutter_output_stream_info_t video;
  double previous_audio_timestamp;
} jumpcutter_output_t;

typedef struct {
  AVPacket* packet;
  jumpcutter_input_t input;
  jumpcutter_output_t output;
  jumpcutter_current_silent_portion_t current_silent_portion;
  jumpcutter_silent_portions_t silent_portions;
  jumpcutter_timestamps_t unprocessed_video_packets;
} jumpcutter_t;

#include "util.h"
#include "silence.h"
#include "lifecycle.h"

int jumpcutter(jumpcutter_t* j, const char* input_filename, const char* output_filename, const float silent_threshold, const uint8_t frame_margin, jumpcutter_error_t* error);

#endif