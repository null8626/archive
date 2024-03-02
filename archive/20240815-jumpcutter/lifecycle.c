#include "jumpcutter.h"
#include <string.h>
#include <float.h>
#include <stdio.h>

int jumpcutter_init(jumpcutter_t* j, const char* input_filename, const char* output_filename, jumpcutter_error_t* error) {
  int ret;
  memset(j, 0, sizeof(jumpcutter_t));
  
  av_log_set_level(AV_LOG_WARNING);
  
  if ((j->packet = av_packet_alloc()) == NULL) {
    error->message = "unable to allocate ffmpeg packet";
    
    return AVERROR_UNKNOWN;
  } else if ((j->input.audio_frame = av_frame_alloc()) == NULL) {
    error->message = "unable to allocate ffmpeg input audio frame";
    
    return AVERROR_UNKNOWN;
  } else if ((ret = avformat_open_input(&j->input.context, input_filename, 0, 0)) < 0) {
    error->message = "unable to open input file";
    
    return ret;
  } else if ((ret = avformat_find_stream_info(j->input.context, 0)) < 0) {
    error->message = "unable to retrieve stream information from input file";
    
    return ret;
  } else if (!jumpcutter_silent_portions_new(&j->silent_portions, error) || !jumpcutter_timestamps_new(&j->unprocessed_video_packets, error)) {
    return AVERROR_UNKNOWN;
  }
  
  av_dump_format(j->input.context, 0, input_filename, 0);
  avformat_alloc_output_context2(&j->output.context, NULL, NULL, output_filename);

  if (j->output.context == NULL) {
    error->message = "unable to create ffmpeg output context";
    
    return AVERROR_UNKNOWN;
  }
  
  int current_output_stream_id = 0;
  
  for (int i = 0; i < j->input.context->nb_streams && (j->output.audio.stream == NULL || j->output.video.stream == NULL); i++) {
    AVStream* stream = j->input.context->streams[i];
    
    if (stream->codecpar->codec_type == AVMEDIA_TYPE_AUDIO && j->output.audio.stream == NULL) {
      const AVCodec* input_audio_codec;
      
      j->input.audio.stream_id = i;
      j->input.audio.stream = stream;
      j->output.audio.stream_id = current_output_stream_id++;

      if ((j->output.audio.stream = avformat_new_stream(j->output.context, NULL)) == NULL) {
        error->message = "unable to create output audio stream";
        
        return AVERROR_UNKNOWN;
      } else if ((input_audio_codec = avcodec_find_decoder(stream->codecpar->codec_id)) == NULL) {
        error->message = "unable to find a proper decoder for the codec used by this input's audio stream";
        
        return AVERROR_UNKNOWN;
      } else if ((j->input.audio_codec_context = avcodec_alloc_context3(input_audio_codec)) == NULL) {
        error->message = "unable to allocate audio codec decoder context";
        
        return AVERROR_UNKNOWN;
      } else if ((ret = avcodec_parameters_to_context(j->input.audio_codec_context, stream->codecpar)) < 0) {
        error->message = "unable to configure audio codec decoder context";
        
        return ret;
      }
      
      j->input.audio_codec_context->request_sample_fmt = av_get_alt_sample_fmt(j->input.audio_codec_context->sample_fmt, 0);
      
      if ((ret = avcodec_open2(j->input.audio_codec_context, input_audio_codec, NULL)) < 0) {
        error->message = "unable to open audio codec decoder and its respective context";
        
        return ret;
      } else if ((ret = avcodec_parameters_copy(j->output.audio.stream->codecpar, stream->codecpar)) < 0) {
        error->message = "unable to copy input audio stream parameters";
        
        return ret;
      }
      
      j->output.audio.stream->codecpar->codec_tag = 0;
    } else if (stream->codecpar->codec_type == AVMEDIA_TYPE_VIDEO && j->output.video.stream == NULL) {
      j->input.video.stream_id = i;
      j->input.video.stream = stream;
      j->output.video.stream_id = current_output_stream_id++;
      
      if ((j->output.video.stream = avformat_new_stream(j->output.context, NULL)) == NULL) {
        error->message = "unable to create output video stream";
        
        return ret;
      } else if ((ret = avcodec_parameters_copy(j->output.video.stream->codecpar, stream->codecpar)) < 0) {
        error->message = "unable to copy input video stream parameters";
        
        return ret;
      }
      
      j->output.video.stream->codecpar->codec_tag = 0;
    }
  }
  
  if (j->output.audio.stream == NULL || j->output.video.stream == NULL) {
    error->message = "unable to find video and audio streams in the input file";
    
    return AVERROR_UNKNOWN;
  }
  
  av_dump_format(j->output.context, 0, output_filename, 1);

  if ((j->output.context->oformat->flags & AVFMT_NOFILE) == 0 && (ret = avio_open(&j->output.context->pb, output_filename, AVIO_FLAG_WRITE)) < 0) {
    error->message = "unable to open output file";
    
    return AVERROR_UNKNOWN;
  }
  
  if ((ret = avformat_write_header(j->output.context, NULL)) < 0) {
    error->message = "unable to write header data to the output file";
    
    return ret;
  }
  
  j->output.audio.packet_data.time_base = av_q2d(j->output.audio.stream->time_base);
  j->output.video.packet_data.time_base = av_q2d(j->output.video.stream->time_base);
  
  j->output.previous_audio_timestamp = -DBL_MAX;
  jumpcutter_current_silent_portion_new(&j->current_silent_portion);
  
  return 0;
}

void jumpcutter_free(jumpcutter_t* j) {
  if (j->output.context != NULL) {
    if ((j->output.context->oformat->flags & AVFMT_NOFILE) == 0) {
      avio_closep(&j->output.context->pb);
    }
    
    avformat_free_context(j->output.context);
  }
  
  jumpcutter_timestamps_free(&j->unprocessed_video_packets);
  jumpcutter_silent_portions_free(&j->silent_portions);

  if (j->input.audio_codec_context != NULL) {
    avcodec_free_context(&j->input.audio_codec_context);
    avcodec_close(j->input.audio_codec_context);
  }
  
  if (j->input.context != NULL) {
    avformat_close_input(&j->input.context);
  }
  
  if (j->input.audio_frame != NULL) {
    av_frame_free(&j->input.audio_frame);
  }
  
  if (j->packet != NULL) {
    av_packet_free(&j->packet);
  }
}