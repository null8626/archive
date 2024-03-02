#include "jumpcutter.h"
#include <float.h>
#include <math.h>

static float get_raw_audio_sample(AVCodecContext* audio_codec_context, void* buf, const int sample_index) {
  const int sample_size = av_get_bytes_per_sample(audio_codec_context->sample_fmt);
  int64_t value;
  
  switch (sample_size) {
    case 1: {
      value = (int64_t)(((uint8_t*)buf)[sample_index] - 127);
      break;
    }
    
    case 2: {
      value = (int64_t)((int16_t*)buf)[sample_index];
      break;
    }
    
    case 4: {
      value = (int64_t)((int32_t*)buf)[sample_index];
      break;
    }
    
    case 8: {
      value = ((int64_t*)buf)[sample_index];
      break;
    }
    
    default: {
      return FLT_MAX;
    }
  }
  
  switch (audio_codec_context->sample_fmt) {
    case AV_SAMPLE_FMT_U8:
    case AV_SAMPLE_FMT_S16:
    case AV_SAMPLE_FMT_S32:
    case AV_SAMPLE_FMT_U8P:
    case AV_SAMPLE_FMT_S16P:
    case AV_SAMPLE_FMT_S32P: {
      return (float)value / (float)(((1 << (sample_size * 8 - 1)) - 1));
    }
    
    case AV_SAMPLE_FMT_FLT:
    case AV_SAMPLE_FMT_FLTP: {
      return *(float*)(&value);
    }
    
    case AV_SAMPLE_FMT_DBL:
    case AV_SAMPLE_FMT_DBLP: {
      return (float)(*(double*)(&value));
    }
    
    default: {
      return FLT_MAX;
    }
  }
}

static int get_audio_sample(jumpcutter_input_t* ji, float* out) {
  int samples_count = 0;
  float samples = 0.0f;
  int ret = 0;
  
  while ((ret = avcodec_receive_frame(ji->audio_codec_context, ji->audio_frame)) >= 0) {
    for (int sample_index = 0; sample_index < ji->audio_frame->nb_samples; sample_index++) {
      for (int channel = 0; channel < ji->audio_codec_context->ch_layout.nb_channels; channel++) {
        float sample = get_raw_audio_sample(ji->audio_codec_context, ji->audio_frame->extended_data[channel], sample_index);
        
        if (sample != FLT_MAX) {
          samples += sample;
          samples_count++;
        }
      }
    }
    
    av_frame_unref(ji->audio_frame);
  }
  
  *out = samples_count == 0 ? FLT_MAX : (samples / (float)samples_count);
  return ret == AVERROR(EAGAIN) || ret == AVERROR_EOF ? 0 : ret;
}

static bool flush_unprocessed_video_packets(jumpcutter_t* j, jumpcutter_error_t* error) {
  jumpcutter_silent_portion_t* current_portion = &j->current_silent_portion.portion;

  for (size_t i = 0; i < j->unprocessed_video_packets.length; i++) {
    const double video_timestamp = j->unprocessed_video_packets.timestamps[i];
    
    if (video_timestamp >= current_portion->start && video_timestamp < current_portion->end && !jumpcutter_timestamps_push(&current_portion->video_packets, video_timestamp, error)) {
      return false;
    }
  }
  
  jumpcutter_timestamps_clear(&j->unprocessed_video_packets);

  if (current_portion->audio_packets.length <= 1) {
    jumpcutter_silent_portion_free(current_portion);
    
    return true;
  }
  
  return jumpcutter_silent_portions_push(&j->silent_portions, current_portion, error);
}

bool jumpcutter_skip_frame(jumpcutter_silent_portions_t* portions, size_t* index, const double timestamp, const uint8_t frame_margin) {
  size_t i = *index;
  
  while (i < portions->length) {
    jumpcutter_silent_portion_t* current_portion = &portions->portions[i];
    
    if (frame_margin > 0) {
      size_t current_portion_size = current_portion->video_packets.length;
      
      if (current_portion_size <= ((size_t)frame_margin * 2)) {
        i++;
        continue;
      }
      
      current_portion->start = current_portion->video_packets.timestamps[frame_margin];
      current_portion->end = current_portion->video_packets.timestamps[current_portion_size - frame_margin];
    }
    
    if (timestamp >= current_portion->start) {
      if (timestamp < current_portion->end) {
        *index = i;
        return true;
      }
      
      i++;
      continue;
    }
    
    break;
  }
  
  return false;
}

int jumpcutter_process_initial_audio_stream(jumpcutter_t* j, const float silent_threshold, jumpcutter_error_t* error) {
  float sample;
  int ret;
  
  jumpcutter_initiate_output_stream_info(j->packet, &j->output.audio, &j->input.audio);
  
  if ((ret = avcodec_send_packet(j->input.audio_codec_context, j->packet)) < 0) {
    error->message = "unable to send audio packet to audio codec decoder";
    
    return ret;
  } else if ((ret = get_audio_sample(&j->input, &sample)) < 0) {
    error->message = "unable to process audio packet samples";
    
    return ret;
  }
  
  jumpcutter_silent_portion_t* current_portion = &j->current_silent_portion.portion;
  double timestamp = jumpcutter_packet_data_get_timestamp(&j->output.audio.packet_data);
  const bool is_first = j->output.video.packet_data.duration == 0;
  const uint8_t is_silent = (uint8_t)(fabs((double)sample) < (double)silent_threshold);
  
  if (is_first) {
    timestamp = -DBL_MAX;
  }
  
  j->output.previous_audio_timestamp = timestamp;
  
  if (j->current_silent_portion.previously_is_silent != is_silent) {
    if ((bool)is_silent) {
      if (!jumpcutter_silent_portion_new(current_portion, timestamp, error)) {
        return AVERROR_UNKNOWN;
      }
    } else if (!is_first) {
      current_portion->end = timestamp;
        
      if (!flush_unprocessed_video_packets(j, error) || !jumpcutter_silent_portion_new(current_portion, DBL_MAX, error)) {
        return AVERROR_UNKNOWN;
      }
      
      goto JUMPCUTTER_PROCESS_INITIAL_AUDIO_STREAM_NOT_SILENT; // code vomit
    } else {
      goto JUMPCUTTER_PROCESS_INITIAL_AUDIO_STREAM_NOT_SILENT;
    }
  } else if (!((bool)is_silent)) {
    goto JUMPCUTTER_PROCESS_INITIAL_AUDIO_STREAM_NOT_SILENT;
  }
  
  if (!jumpcutter_timestamps_push(&current_portion->audio_packets, timestamp, error)) {
    return AVERROR_UNKNOWN;
  }

JUMPCUTTER_PROCESS_INITIAL_AUDIO_STREAM_NOT_SILENT:
  j->current_silent_portion.previously_is_silent = is_silent;
  j->output.audio.packet_data.index++;
  
  return 0;
}

int jumpcutter_process_initial_video_stream(jumpcutter_t* j, jumpcutter_error_t* error) {
  int ret;
  
  jumpcutter_initiate_output_stream_info(j->packet, &j->output.video, &j->input.video);
  
  const double timestamp = jumpcutter_packet_data_get_timestamp(&j->output.video.packet_data);
  jumpcutter_silent_portion_t* portion;
  size_t i = j->silent_portions.length;
  
  if (i > 0) {
    do {
      i--;
      portion = &j->silent_portions.portions[i];
      
      if (timestamp >= portion->start) {
        if (timestamp < portion->end) {
          if (!jumpcutter_timestamps_push(&portion->video_packets, timestamp, error)) {
            return AVERROR_UNKNOWN;
          }
          
          goto JUMPCUTTER_PROCESS_INITIAL_VIDEO_STREAM_END;
        }
        
        break;
      }
    } while (i > 0);
  }
  
  if ((j->output.audio.packet_data.duration == 0 || timestamp > j->output.previous_audio_timestamp || j->current_silent_portion.portion.start != DBL_MAX) && !jumpcutter_timestamps_push(&j->unprocessed_video_packets, timestamp, error)) {
    return AVERROR_UNKNOWN;
  }
  
JUMPCUTTER_PROCESS_INITIAL_VIDEO_STREAM_END:
  j->output.video.packet_data.index++;

  return 0;
}

bool jumpcutter_flush_after_initial_read(jumpcutter_t* j, jumpcutter_error_t* error) {
  jumpcutter_silent_portion_t* current_portion = &j->current_silent_portion.portion;
  
  j->output.audio.packet_data.index = 0;
  j->output.video.packet_data.index = 0;
  
  if (current_portion->start != DBL_MAX) {
    current_portion->end = DBL_MAX;
    
    return flush_unprocessed_video_packets(j, error);
  }
  
  return true;
}