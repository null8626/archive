#include "jumpcutter.h"
#include <stdlib.h>
#include <string.h>

typedef struct {
  void* buf;
  size_t length;
  size_t capacity;
} jumpcutter_vector_t;

static bool vector_new(jumpcutter_vector_t* vec, const size_t initial_capacity, const size_t unit_size, jumpcutter_error_t* error) {
  vec->length = 0;
  vec->capacity = initial_capacity;

  if ((vec->buf = malloc(vec->capacity * unit_size)) != NULL) {
    memset(vec->buf, 0, vec->capacity * unit_size);
    
    return true;
  }

  error->message = "out of memory";
  
  return false;
}

static bool vector_push(jumpcutter_vector_t* vec, const void* unit, const size_t unit_size, jumpcutter_error_t* error) {
  if (vec->buf != NULL) {
    if (vec->length == vec->capacity) {
      vec->capacity *= 2;
      
      if ((vec->buf = realloc(vec->buf, vec->capacity * unit_size)) == NULL) {
        error->message = "out of memory";
        
        return false;
      }
    }
    
    memcpy(((uint8_t*)vec->buf) + (vec->length * unit_size), unit, unit_size);
    vec->length++;
  }
  
  return true;
}

static void vector_free(jumpcutter_vector_t* vec) {
  if (vec->buf != NULL) {
    free(vec->buf);
    vec->buf = NULL;
  }
}

void jumpcutter_packet_data_apply(jumpcutter_packet_data_t* d, AVPacket* packet) {
  const int64_t addition = (int64_t)d->index * d->duration;
  
  packet->pts = d->pts + addition;
  packet->dts = d->dts + addition;
  packet->duration = d->duration;
  d->index++;
}

double jumpcutter_packet_data_get_timestamp(jumpcutter_packet_data_t* d) {
  return (double)(d->dts + ((int64_t)d->index * d->duration)) * d->time_base;
}

void jumpcutter_initiate_output_stream_info(const AVPacket* packet, jumpcutter_output_stream_info_t* output, jumpcutter_input_stream_info_t* input) {
  if (output->packet_data.duration == 0) {
    output->packet_data.pts = av_rescale_q_rnd(packet->pts, input->stream->time_base, output->stream->time_base, AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX);
    output->packet_data.dts = av_rescale_q_rnd(packet->dts, input->stream->time_base, output->stream->time_base, AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX);
    output->packet_data.duration = av_rescale_q(packet->duration, input->stream->time_base, output->stream->time_base);
  }
}

bool jumpcutter_silent_portions_new(jumpcutter_silent_portions_t* portions, jumpcutter_error_t* error) {
  return vector_new((jumpcutter_vector_t*)portions, 16, sizeof(jumpcutter_silent_portion_t), error);
}

bool jumpcutter_silent_portions_push(jumpcutter_silent_portions_t* portions, const jumpcutter_silent_portion_t* portion, jumpcutter_error_t* error) {
  return vector_push((jumpcutter_vector_t*)portions, (const void*)portion, sizeof(jumpcutter_silent_portion_t), error);
}

void jumpcutter_silent_portions_free(jumpcutter_silent_portions_t* portions) {
  for (size_t i = 0; i < portions->length; i++) {
    jumpcutter_silent_portion_free(&portions->portions[i]);
  }
  
  vector_free((jumpcutter_vector_t*)portions);
}

bool jumpcutter_silent_portion_new(jumpcutter_silent_portion_t* portion, const double start, jumpcutter_error_t* error) {
  portion->start = start;
  
  if (error == NULL) {
    return true;
  }
  
  return jumpcutter_timestamps_new(&portion->audio_packets, error) &&
         jumpcutter_timestamps_new(&portion->video_packets, error);
}

bool jumpcutter_timestamps_new(jumpcutter_timestamps_t* timestamps, jumpcutter_error_t* error) {
  return vector_new((jumpcutter_vector_t*)timestamps, 128, sizeof(double), error);
}

bool jumpcutter_timestamps_push(jumpcutter_timestamps_t* timestamps, const double timestamp, jumpcutter_error_t* error) {
  return vector_push((jumpcutter_vector_t*)timestamps, (const void*)&timestamp, sizeof(double), error);
}

void jumpcutter_timestamps_free(jumpcutter_timestamps_t* timestamps) {
  vector_free((jumpcutter_vector_t*)timestamps);
}