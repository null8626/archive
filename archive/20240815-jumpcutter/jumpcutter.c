#include "jumpcutter.h"
#include <stdio.h>

int jumpcutter(jumpcutter_t* j, const char* input_filename, const char* output_filename, const float silent_threshold, const uint8_t frame_margin, jumpcutter_error_t* error) {
  int ret;
  
  if ((ret = jumpcutter_init(j, input_filename, output_filename, error)) < 0) {
    return ret;
  }
  
  while ((ret = av_read_frame(j->input.context, j->packet)) >= 0) {
    if (j->packet->stream_index == j->input.audio.stream_id) {
      if ((ret = jumpcutter_process_initial_audio_stream(j, silent_threshold, error)) < 0) {
        av_packet_unref(j->packet);
        
        return ret;
      }
    } else if (j->packet->stream_index == j->input.video.stream_id) {
      if ((ret = jumpcutter_process_initial_video_stream(j, error)) < 0) {
        av_packet_unref(j->packet);
        
        return ret;
      }
    }
    
    av_packet_unref(j->packet);
  }
  
  if (ret != AVERROR_EOF) {
    return ret;
  } else if (!jumpcutter_flush_after_initial_read(j, error)) {
    return AVERROR_UNKNOWN;
  }
  
  jumpcutter_packet_data_t skipped_audio_packet_data;
  jumpcutter_packet_data_t skipped_video_packet_data;
  
  memcpy(&skipped_audio_packet_data, &j->output.audio.packet_data, sizeof(jumpcutter_packet_data_t));
  memcpy(&skipped_video_packet_data, &j->output.video.packet_data, sizeof(jumpcutter_packet_data_t));
  
  for (int i = 0; i < j->input.context->nb_streams; i++) {
    if ((ret = av_seek_frame(j->input.context, i, 0, AVSEEK_FLAG_BACKWARD)) < 0) {
      error->message = "unable to seek input stream back to beginning";
    
      return ret;
    }
  }
  
  jumpcutter_packet_data_t* current_skipped_packet_data;
  jumpcutter_packet_data_t* current_unskipped_packet_data;
  size_t current_silent_portion_index = 0;
  
  while ((ret = av_read_frame(j->input.context, j->packet)) >= 0) {
    if (j->packet->stream_index == j->input.audio.stream_id) {
      current_skipped_packet_data = &skipped_audio_packet_data;
      current_unskipped_packet_data = &j->output.audio.packet_data;
    } else if (j->packet->stream_index == j->input.video.stream_id) {
      current_skipped_packet_data = &skipped_video_packet_data;
      current_unskipped_packet_data = &j->output.video.packet_data;
    } else {
      av_packet_unref(j->packet);
      
      continue;
    }
    
    if (jumpcutter_skip_frame(
      &j->silent_portions,
      &current_silent_portion_index,
      jumpcutter_packet_data_get_timestamp(current_unskipped_packet_data),
      frame_margin
    )) {
      av_packet_unref(j->packet);
      current_unskipped_packet_data->index++;
      
      continue;
    }
    
    jumpcutter_packet_data_apply(current_skipped_packet_data, j->packet);
    current_unskipped_packet_data->index++;
    
    if ((ret = av_interleaved_write_frame(j->output.context, j->packet)) < 0) {
      error->message = "unable to write altered packet to output file";
      av_packet_unref(j->packet);

      return ret;
    }
  }
  
  if (ret == AVERROR_EOF) {
    av_write_trailer(j->output.context);
    
    return 0;
  }
  
  return ret;
}

int main(int argc, char **argv) {
  if (argc < 3) {
    fprintf(stderr, "usage: %s input output\n", argv[0]);
    return 1;
  }
  
  jumpcutter_error_t error = {NULL};
  jumpcutter_t j;
  
  const int ret = jumpcutter(&j, argv[1], argv[2], 0.0001f, 2, &error);
  int exit_code = 0;
  
  jumpcutter_free(&j);
  
  if (error.message != NULL) {
    fprintf(stderr, "error: %s\n", error.message);
    exit_code = 1;
  }
  
  if (ret < 0 && ret != AVERROR_UNKNOWN && ret != AVERROR_EOF) {
    fprintf(stderr, "ffmpeg error: \"%s\"\n", av_err2str(ret));
    exit_code = 1;
  }
  
  if (exit_code == 0) {
    printf("ok.\n");
  }

  return exit_code;
}