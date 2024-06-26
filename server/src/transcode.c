/*
 * Copyright (c) 2010 Nicolas George
 * Copyright (c) 2011 Stefano Sabatini
 * Copyright (c) 2014 Andrey Utkin
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

/**
 * @file demuxing, decoding, filtering, encoding and muxing API usage example
 * @example transcode.c
 *
 * Convert input to output file, applying some hard-coded filter-graph on both
 * audio and video streams.
 */

#ifndef transcode_c
#define transcode_c
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libavfilter/buffersink.h"
#include "libavfilter/buffersrc.h"
#include "libavutil/channel_layout.h"
#include "libavutil/opt.h"
#include "libavutil/pixdesc.h"
#include "transcode.h"
#include <stdbool.h>
#endif
typedef struct FilteringContext {
    AVFilterContext *buffersink_ctx;
    AVFilterContext *buffersrc_ctx;
    AVFilterGraph *filter_graph;
    
    AVPacket *enc_pkt;
    AVFrame *filtered_frame;
} FilteringContext;



typedef struct StreamContext {
    AVCodecContext *dec_ctx;
    AVCodecContext *enc_ctx;
    
    AVFrame *dec_frame;
} StreamContext;




typedef struct TranscodeManager {
    FilteringContext *filter_ctx;
    AVFormatContext *ifmt_ctx;
    AVFormatContext *ofmt_ctx;
    StreamContext *stream_ctx;
    long total_frame;
    long compressed_frame;
    ProcessCallBack callBack;
    bool finished;
    char *fileName;
} TranscodeManager;



static int open_input_file(const char *filename, TranscodeManager *manager)
{
    int ret;
    unsigned int i;
    manager->ifmt_ctx = NULL;
    
    
    
    if ((ret = avformat_open_input(&manager->ifmt_ctx, filename, NULL, NULL)) < 0) {
        av_log(NULL, AV_LOG_ERROR, "Cannot open input file\n");
        return ret;
    }
    
    if ((ret = avformat_find_stream_info(manager->ifmt_ctx, NULL)) < 0) {
        av_log(NULL, AV_LOG_ERROR, "Cannot find stream information\n");
        return ret;
    }
    
    manager->stream_ctx = av_calloc(manager->ifmt_ctx->nb_streams, sizeof(*manager->stream_ctx));
    if (!manager->stream_ctx)
        return AVERROR(ENOMEM);
    
    for (i = 0; i < manager->ifmt_ctx->nb_streams; i++) {
        AVStream *stream = manager->ifmt_ctx->streams[i];
        const AVCodec *dec = avcodec_find_decoder(stream->codecpar->codec_id);
        AVCodecContext *codec_ctx;
        if (!dec) {
            av_log(NULL, AV_LOG_ERROR, "Failed to find decoder for stream #%u\n", i);
            return AVERROR_DECODER_NOT_FOUND;
        }
        codec_ctx = avcodec_alloc_context3(dec);
        if (!codec_ctx) {
            av_log(NULL, AV_LOG_ERROR, "Failed to allocate the decoder context for stream #%u\n", i);
            return AVERROR(ENOMEM);
        }
        ret = avcodec_parameters_to_context(codec_ctx, stream->codecpar);
        if (ret < 0) {
            av_log(NULL, AV_LOG_ERROR, "Failed to copy decoder parameters to input decoder context "
                   "for stream #%u\n", i);
            return ret;
        }
        
        
        
        /* Inform the decoder about the timebase for the packet timestamps.
         * This is highly recommended, but not mandatory. */
        codec_ctx->pkt_timebase = stream->time_base;
        
        /* Reencode video & audio and remux subtitles etc. */
        if (codec_ctx->codec_type == AVMEDIA_TYPE_VIDEO
            || codec_ctx->codec_type == AVMEDIA_TYPE_AUDIO) {
            if (codec_ctx->codec_type == AVMEDIA_TYPE_VIDEO)
                codec_ctx->framerate = av_guess_frame_rate(manager->ifmt_ctx, stream, NULL);
            /* Open decoder */
            ret = avcodec_open2(codec_ctx, dec, NULL);
            
            if (ret < 0) {
                av_log(NULL, AV_LOG_ERROR, "Failed to open decoder for stream #%u\n", i);
                return ret;
            }
        }
        manager->stream_ctx[i].dec_ctx = codec_ctx;
        
        manager->stream_ctx[i].dec_frame = av_frame_alloc();
        if (!manager->stream_ctx[i].dec_frame)
            return AVERROR(ENOMEM);
    }
    
    av_dump_format(manager->ifmt_ctx, 0, filename, 0);
    return 0;
}

static int open_output_file(const char *filename, TranscodeManager *manager, char *crf, char *preset)
{
    printf("--压缩参数是crf:%s preset:%s",crf, preset);
    AVStream *out_stream;
    AVStream *in_stream;
    AVCodecContext *dec_ctx, *enc_ctx;
    const AVCodec *encoder;
    int ret;
    unsigned int i;
    
    manager->ofmt_ctx = NULL;
    avformat_alloc_output_context2(&manager->ofmt_ctx, NULL, NULL, filename);
    if (!manager->ofmt_ctx) {
        av_log(NULL, AV_LOG_ERROR, "Could not create output context\n");
        return AVERROR_UNKNOWN;
    }
    
    
    for (i = 0; i < manager->ifmt_ctx->nb_streams; i++) {
        out_stream = avformat_new_stream(manager->ofmt_ctx, NULL);
        if (!out_stream) {
            av_log(NULL, AV_LOG_ERROR, "Failed allocating output stream\n");
            return AVERROR_UNKNOWN;
        }
        
        in_stream = manager->ifmt_ctx->streams[i];
        dec_ctx = manager->stream_ctx[i].dec_ctx;
        
        if (dec_ctx->codec_type == AVMEDIA_TYPE_VIDEO
            || dec_ctx->codec_type == AVMEDIA_TYPE_AUDIO) {
            encoder = avcodec_find_encoder(dec_ctx->codec_id);
            /* in this example, we choose transcoding to same codec */
            if (dec_ctx->codec_type == AVMEDIA_TYPE_VIDEO) {
                encoder = avcodec_find_encoder_by_name("libx264");
                if (encoder == NULL) {
                    encoder = avcodec_find_encoder(AV_CODEC_ID_H264);
                }
            }else {
                encoder = avcodec_find_encoder(AV_CODEC_ID_AAC);
            }
            if (!encoder) {
                av_log(NULL, AV_LOG_FATAL, "Necessary encoder not found\n");
                return AVERROR_INVALIDDATA;
            }
            enc_ctx = avcodec_alloc_context3(encoder);
            if (!enc_ctx) {
                av_log(NULL, AV_LOG_FATAL, "Failed to allocate the encoder context\n");
                return AVERROR(ENOMEM);
            }
            
            /* In this example, we transcode to same properties (picture size,
             * sample rate etc.). These properties can be changed for output
             * streams easily using filters */
            if (dec_ctx->codec_type == AVMEDIA_TYPE_VIDEO) {
                enc_ctx->height = dec_ctx->height;
                enc_ctx->width = dec_ctx->width;
                enc_ctx->sample_aspect_ratio = dec_ctx->sample_aspect_ratio;
                /* take first format from list of supported formats */
                if (encoder->pix_fmts)
                    enc_ctx->pix_fmt = encoder->pix_fmts[0];
                else
                    enc_ctx->pix_fmt = dec_ctx->pix_fmt;
                /* video time_base can be set to whatever is handy and supported by encoder */
                enc_ctx->time_base = av_inv_q(dec_ctx->framerate);
                
            } else {
                enc_ctx->sample_rate = dec_ctx->sample_rate;
                ret = av_channel_layout_copy(&enc_ctx->ch_layout, &dec_ctx->ch_layout);
                if (ret < 0)
                    return ret;
                /* take first format from list of supported formats */
                enc_ctx->sample_fmt = encoder->sample_fmts[0];
                enc_ctx->time_base = (AVRational){1, enc_ctx->sample_rate};
            }
            
            if (manager->ofmt_ctx->oformat->flags & AVFMT_GLOBALHEADER)
                enc_ctx->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
            
            /* Third parameter can be used to pass settings to encoder */
            ret = avcodec_open2(enc_ctx, encoder, NULL);
            av_opt_set(enc_ctx->priv_data, "preset", preset, 0);
            av_opt_set(enc_ctx->priv_data, "crf", crf, 0);
            if (ret < 0) {
                av_log(NULL, AV_LOG_ERROR, "Cannot open %s encoder for stream #%u\n", encoder->name, i);
                return ret;
            }
            ret = avcodec_parameters_from_context(out_stream->codecpar, enc_ctx);
            if (ret < 0) {
                av_log(NULL, AV_LOG_ERROR, "Failed to copy encoder parameters to output stream #%u\n", i);
                return ret;
            }
            
            out_stream->time_base = enc_ctx->time_base;
            manager->stream_ctx[i].enc_ctx = enc_ctx;
        } else if (dec_ctx->codec_type == AVMEDIA_TYPE_UNKNOWN) {
            av_log(NULL, AV_LOG_FATAL, "Elementary stream #%d is of unknown type, cannot proceed\n", i);
            return AVERROR_INVALIDDATA;
        } else {
            /* if this stream must be remuxed */
            ret = avcodec_parameters_copy(out_stream->codecpar, in_stream->codecpar);
            if (ret < 0) {
                av_log(NULL, AV_LOG_ERROR, "Copying parameters for stream #%u failed\n", i);
                return ret;
            }
            out_stream->time_base = in_stream->time_base;
        }
        
    }
    av_dump_format(manager->ofmt_ctx, 0, filename, 1);
    
    if (!(manager->ofmt_ctx->oformat->flags & AVFMT_NOFILE)) {
        ret = avio_open(&manager->ofmt_ctx->pb, filename, AVIO_FLAG_WRITE);
        if (ret < 0) {
            av_log(NULL, AV_LOG_ERROR, "Could not open output file '%s'", filename);
            return ret;
        }
    }
    
    /* init muxer, write output file header */
    ret = avformat_write_header(manager->ofmt_ctx, NULL);
    if (ret < 0) {
        av_log(NULL, AV_LOG_ERROR, "Error occurred when opening output file\n");
        return ret;
    }
    
    return 0;
}

static int init_filter(FilteringContext* fctx, AVCodecContext *dec_ctx,
                       AVCodecContext *enc_ctx, const char *filter_spec, TranscodeManager *manager)
{
    char args[512];
    int ret = 0;
    const AVFilter *buffersrc = NULL;
    const AVFilter *buffersink = NULL;
    AVFilterContext *buffersrc_ctx = NULL;
    AVFilterContext *buffersink_ctx = NULL;
    AVFilterInOut *outputs = avfilter_inout_alloc();
    AVFilterInOut *inputs  = avfilter_inout_alloc();
    AVFilterGraph *filter_graph = avfilter_graph_alloc();
    
    if (!outputs || !inputs || !filter_graph) {
        ret = AVERROR(ENOMEM);
        goto end;
    }
    
    if (dec_ctx->codec_type == AVMEDIA_TYPE_VIDEO) {
        buffersrc = avfilter_get_by_name("buffer");
        buffersink = avfilter_get_by_name("buffersink");
        if (!buffersrc || !buffersink) {
            av_log(NULL, AV_LOG_ERROR, "filtering source or sink element not found\n");
            ret = AVERROR_UNKNOWN;
            goto end;
        }
        
        snprintf(args, sizeof(args),
                 "video_size=%dx%d:pix_fmt=%d:time_base=%d/%d:pixel_aspect=%d/%d",
                 dec_ctx->width, dec_ctx->height, dec_ctx->pix_fmt,
                 dec_ctx->pkt_timebase.num, dec_ctx->pkt_timebase.den,
                 dec_ctx->sample_aspect_ratio.num,
                 dec_ctx->sample_aspect_ratio.den);
        
        ret = avfilter_graph_create_filter(&buffersrc_ctx, buffersrc, "in",
                                           args, NULL, filter_graph);
        if (ret < 0) {
            av_log(NULL, AV_LOG_ERROR, "Cannot create buffer source\n");
            goto end;
        }
        
        ret = avfilter_graph_create_filter(&buffersink_ctx, buffersink, "out",
                                           NULL, NULL, filter_graph);
        if (ret < 0) {
            av_log(NULL, AV_LOG_ERROR, "Cannot create buffer sink\n");
            goto end;
        }
        
        ret = av_opt_set_bin(buffersink_ctx, "pix_fmts",
                             (uint8_t*)&enc_ctx->pix_fmt, sizeof(enc_ctx->pix_fmt),
                             AV_OPT_SEARCH_CHILDREN);
        if (ret < 0) {
            av_log(NULL, AV_LOG_ERROR, "Cannot set output pixel format\n");
            goto end;
        }
    } else if (dec_ctx->codec_type == AVMEDIA_TYPE_AUDIO) {
        char buf[64];
        buffersrc = avfilter_get_by_name("abuffer");
        buffersink = avfilter_get_by_name("abuffersink");
        if (!buffersrc || !buffersink) {
            av_log(NULL, AV_LOG_ERROR, "filtering source or sink element not found\n");
            ret = AVERROR_UNKNOWN;
            goto end;
        }
        
        if (dec_ctx->ch_layout.order == AV_CHANNEL_ORDER_UNSPEC)
            av_channel_layout_default(&dec_ctx->ch_layout, dec_ctx->ch_layout.nb_channels);
        av_channel_layout_describe(&dec_ctx->ch_layout, buf, sizeof(buf));
        snprintf(args, sizeof(args),
                 "time_base=%d/%d:sample_rate=%d:sample_fmt=%s:channel_layout=%s",
                 dec_ctx->pkt_timebase.num, dec_ctx->pkt_timebase.den, dec_ctx->sample_rate,
                 av_get_sample_fmt_name(dec_ctx->sample_fmt),
                 buf);
        ret = avfilter_graph_create_filter(&buffersrc_ctx, buffersrc, "in",
                                           args, NULL, filter_graph);
        if (ret < 0) {
            av_log(NULL, AV_LOG_ERROR, "Cannot create audio buffer source\n");
            goto end;
        }
        
        ret = avfilter_graph_create_filter(&buffersink_ctx, buffersink, "out",
                                           NULL, NULL, filter_graph);
        if (ret < 0) {
            av_log(NULL, AV_LOG_ERROR, "Cannot create audio buffer sink\n");
            goto end;
        }
        
        ret = av_opt_set_bin(buffersink_ctx, "sample_fmts",
                             (uint8_t*)&enc_ctx->sample_fmt, sizeof(enc_ctx->sample_fmt),
                             AV_OPT_SEARCH_CHILDREN);
        if (ret < 0) {
            av_log(NULL, AV_LOG_ERROR, "Cannot set output sample format\n");
            goto end;
        }
        
        av_channel_layout_describe(&enc_ctx->ch_layout, buf, sizeof(buf));
        ret = av_opt_set(buffersink_ctx, "ch_layouts",
                         buf, AV_OPT_SEARCH_CHILDREN);
        if (ret < 0) {
            av_log(NULL, AV_LOG_ERROR, "Cannot set output channel layout\n");
            goto end;
        }
        
        ret = av_opt_set_bin(buffersink_ctx, "sample_rates",
                             (uint8_t*)&enc_ctx->sample_rate, sizeof(enc_ctx->sample_rate),
                             AV_OPT_SEARCH_CHILDREN);
        if (ret < 0) {
            av_log(NULL, AV_LOG_ERROR, "Cannot set output sample rate\n");
            goto end;
        }
    } else {
        ret = AVERROR_UNKNOWN;
        goto end;
    }
    
    /* Endpoints for the filter graph. */
    outputs->name       = av_strdup("in");
    outputs->filter_ctx = buffersrc_ctx;
    outputs->pad_idx    = 0;
    outputs->next       = NULL;
    
    inputs->name       = av_strdup("out");
    inputs->filter_ctx = buffersink_ctx;
    inputs->pad_idx    = 0;
    inputs->next       = NULL;
    
    if (!outputs->name || !inputs->name) {
        ret = AVERROR(ENOMEM);
        goto end;
    }
    
    if ((ret = avfilter_graph_parse_ptr(filter_graph, filter_spec,
                                        &inputs, &outputs, NULL)) < 0)
        goto end;
    
    if ((ret = avfilter_graph_config(filter_graph, NULL)) < 0)
        goto end;
    
    /* Fill FilteringContext */
    fctx->buffersrc_ctx = buffersrc_ctx;
    fctx->buffersink_ctx = buffersink_ctx;
    fctx->filter_graph = filter_graph;
    
end:
    avfilter_inout_free(&inputs);
    avfilter_inout_free(&outputs);
    
    return ret;
}

static int init_filters(TranscodeManager *manager)
{
    const char *filter_spec;
    unsigned int i;
    int ret;
    manager->filter_ctx = av_malloc_array(manager->ifmt_ctx->nb_streams, sizeof(*manager->filter_ctx));
    if (!manager->filter_ctx)
        return AVERROR(ENOMEM);
    
    for (i = 0; i < manager->ifmt_ctx->nb_streams; i++) {
        manager->filter_ctx[i].buffersrc_ctx  = NULL;
        manager->filter_ctx[i].buffersink_ctx = NULL;
        manager->filter_ctx[i].filter_graph   = NULL;
        AVStream *stream = manager->ifmt_ctx->streams[i];
        if (!(stream->codecpar->codec_type == AVMEDIA_TYPE_AUDIO
              || stream->codecpar->codec_type == AVMEDIA_TYPE_VIDEO))
            continue;
        
        if (stream->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            manager->total_frame = stream->nb_frames;
        }
        
        if (manager->ifmt_ctx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO)
            filter_spec = "null"; /* passthrough (dummy) filter for video */
        else
            filter_spec = "anull"; /* passthrough (dummy) filter for audio */
        ret = init_filter(&manager->filter_ctx[i], manager->stream_ctx[i].dec_ctx,
                          manager->stream_ctx[i].enc_ctx, filter_spec, manager);
        if (ret)
            return ret;
        
        manager->filter_ctx[i].enc_pkt = av_packet_alloc();
        if (!manager->filter_ctx[i].enc_pkt)
            return AVERROR(ENOMEM);
        
        manager->filter_ctx[i].filtered_frame = av_frame_alloc();
        if (!manager->filter_ctx[i].filtered_frame)
            return AVERROR(ENOMEM);
    }
    return 0;
}


void managerDealProgress(TranscodeManager *manager, bool isFinish);

static int encode_write_frame(unsigned int stream_index, int flush, TranscodeManager *manager)
{
    StreamContext *stream = &manager->stream_ctx[stream_index];
    FilteringContext *filter = &manager->filter_ctx[stream_index];
    AVFrame *filt_frame = flush ? NULL : filter->filtered_frame;
    AVPacket *enc_pkt = filter->enc_pkt;
    int ret;
    
    av_log(NULL, AV_LOG_INFO, "Encoding frame\n");
    /* encode filtered frame */
    av_packet_unref(enc_pkt);
    
    if (filt_frame && filt_frame->pts != AV_NOPTS_VALUE)
        filt_frame->pts = av_rescale_q(filt_frame->pts, filt_frame->time_base,
                                       stream->enc_ctx->time_base);
    
    ret = avcodec_send_frame(stream->enc_ctx, filt_frame);
    
    if (ret < 0)
        return ret;
    
    while (ret >= 0) {
        ret = avcodec_receive_packet(stream->enc_ctx, enc_pkt);
        if (stream->enc_ctx->codec->type == AVMEDIA_TYPE_VIDEO) {
            manager->compressed_frame = stream->enc_ctx->frame_num;
            managerDealProgress(manager, false);
        }
        
        if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
            return 0;
        
        /* prepare packet for muxing */
        enc_pkt->stream_index = stream_index;
        av_packet_rescale_ts(enc_pkt,
                             stream->enc_ctx->time_base,
                             manager->ofmt_ctx->streams[stream_index]->time_base);
        
        av_log(NULL, AV_LOG_DEBUG, "Muxing frame\n");
        /* mux encoded frame */
        ret = av_interleaved_write_frame(manager->ofmt_ctx, enc_pkt);
    }
    
    return ret;
}

static int filter_encode_write_frame(AVFrame *frame, unsigned int stream_index, TranscodeManager *manager)
{
    FilteringContext *filter = &manager->filter_ctx[stream_index];
    int ret;
    
    av_log(NULL, AV_LOG_INFO, "Pushing decoded frame to filters\n");
    /* push the decoded frame into the filtergraph */
    ret = av_buffersrc_add_frame_flags(filter->buffersrc_ctx,
                                       frame, 0);
    if (ret < 0) {
        av_log(NULL, AV_LOG_ERROR, "Error while feeding the filtergraph\n");
        return ret;
    }
    
    /* pull filtered frames from the filtergraph */
    while (1) {
        av_log(NULL, AV_LOG_INFO, "Pulling filtered frame from filters\n");
        ret = av_buffersink_get_frame(filter->buffersink_ctx,
                                      filter->filtered_frame);
        if (ret < 0) {
            /* if no more frames for output - returns AVERROR(EAGAIN)
             * if flushed and no more frames for output - returns AVERROR_EOF
             * rewrite retcode to 0 to show it as normal procedure completion
             */
            if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
                ret = 0;
            break;
        }
        
        filter->filtered_frame->time_base = av_buffersink_get_time_base(filter->buffersink_ctx);;
        filter->filtered_frame->pict_type = AV_PICTURE_TYPE_NONE;
        ret = encode_write_frame(stream_index, 0, manager);
        av_frame_unref(filter->filtered_frame);
        if (ret < 0)
            break;
    }
    
    return ret;
}

static int flush_encoder(unsigned int stream_index, TranscodeManager *manager)
{
    if (!(manager->stream_ctx[stream_index].enc_ctx->codec->capabilities &
          AV_CODEC_CAP_DELAY))
        return 0;
    
    av_log(NULL, AV_LOG_INFO, "Flushing stream #%u encoder\n", stream_index);
    return encode_write_frame(stream_index, 1, manager);
}

int av_compress_video(char* inputPath, char *outputPath, char*crf, char *preset, ProcessCallBack callBack)
{
    int ret;
    AVPacket *packet = NULL;
    unsigned int stream_index;
    unsigned int i;
    TranscodeManager manager;
    memset(&manager, 0, sizeof(manager));
    if (crf == NULL) {
        crf = "23";
    }
    if (preset == NULL) {
        preset = "slow";
    }
    manager.callBack = callBack;
    manager.fileName = inputPath;
    
    if ((ret = open_input_file(inputPath, &manager)) < 0)
        goto end;
    if ((ret = open_output_file(outputPath, &manager, crf, preset) < 0))
        goto end;
    if ((ret = init_filters(&manager)) < 0)
        goto end;
    if (!(packet = av_packet_alloc()))
        goto end;
    
    /* read all packets */
    int test = 0;
    while (1) {
        
        if ((ret = av_read_frame(manager.ifmt_ctx, packet)) < 0)
            break;
        stream_index = packet->stream_index;
        printf("------%d\n", stream_index);
        av_log(NULL, AV_LOG_INFO, "Demuxer gave frame of stream_index %u\n",
               stream_index);
        FilteringContext *c = &manager.filter_ctx[stream_index];
        
        if (manager.filter_ctx[stream_index].filter_graph) {
            StreamContext *stream = &manager.stream_ctx[stream_index];
            printf("------current deal is %s\n", stream->dec_ctx->codec->long_name);
            av_log(NULL, AV_LOG_INFO, "Going to reencode&filter the frame\n");
            
            ret = avcodec_send_packet(stream->dec_ctx, packet);
            if (ret < 0) {
                av_log(NULL, AV_LOG_ERROR, "Decoding failed\n");
                break;
            }
            
            while (ret >= 0) {
                ret = avcodec_receive_frame(stream->dec_ctx, stream->dec_frame);
                if (ret == AVERROR_EOF || ret == AVERROR(EAGAIN))
                    break;
                else if (ret < 0)
                    goto end;
                
                stream->dec_frame->pts = stream->dec_frame->best_effort_timestamp;
                ret = filter_encode_write_frame(stream->dec_frame, stream_index, &manager);
                if (ret < 0)
                    goto end;
            }
        } else {
            /* remux this frame without reencoding */
            av_packet_rescale_ts(packet,
                                 manager.ifmt_ctx->streams[stream_index]->time_base,
                                 manager.ofmt_ctx->streams[stream_index]->time_base);
            
            ret = av_interleaved_write_frame(manager.ofmt_ctx, packet);
            if (ret < 0)
                goto end;
        }
        av_packet_unref(packet);
    }
    
    /* flush decoders, filters and encoders */
    for (i = 0; i < manager.ifmt_ctx->nb_streams; i++) {
        StreamContext *stream;
        
        if (!manager.filter_ctx[i].filter_graph)
            continue;
        
        stream = &manager.stream_ctx[i];
        
        av_log(NULL, AV_LOG_INFO, "Flushing stream %u decoder\n", i);
        
        /* flush decoder */
        ret = avcodec_send_packet(stream->dec_ctx, NULL);
        if (ret < 0) {
            av_log(NULL, AV_LOG_ERROR, "Flushing decoding failed\n");
            goto end;
        }
        
        while (ret >= 0) {
            ret = avcodec_receive_frame(stream->dec_ctx, stream->dec_frame);
            if (ret == AVERROR_EOF)
                break;
            else if (ret < 0)
                goto end;
            
            stream->dec_frame->pts = stream->dec_frame->best_effort_timestamp;
            ret = filter_encode_write_frame(stream->dec_frame, i, &manager);
            if (ret < 0)
                goto end;
        }
        
        /* flush filter */
        av_log(NULL, AV_LOG_INFO, "flush filter\n");
        ret = filter_encode_write_frame(NULL, i, &manager);
        if (ret < 0) {
            av_log(NULL, AV_LOG_ERROR, "Flushing filter failed\n");
            goto end;
        }
        
        /* flush encoder */
        ret = flush_encoder(i, &manager);
        if (ret < 0) {
            av_log(NULL, AV_LOG_ERROR, "Flushing encoder failed\n");
            goto end;
        }
    }
    
    av_write_trailer(manager.ofmt_ctx);
end:
    av_packet_free(&packet);
    for (i = 0; i < manager.ifmt_ctx->nb_streams; i++) {
        avcodec_free_context(&manager.stream_ctx[i].dec_ctx);
        if (manager.ofmt_ctx && manager.ofmt_ctx->nb_streams > i && manager.ofmt_ctx->streams[i] && manager.stream_ctx[i].enc_ctx)
            avcodec_free_context(&manager.stream_ctx[i].enc_ctx);
        if (manager.filter_ctx && manager.filter_ctx[i].filter_graph) {
            avfilter_graph_free(&manager.filter_ctx[i].filter_graph);
            av_packet_free(&manager.filter_ctx[i].enc_pkt);
            av_frame_free(&manager.filter_ctx[i].filtered_frame);
        }
        
        av_frame_free(&manager.stream_ctx[i].dec_frame);
    }
    av_free(manager.filter_ctx);
    av_free(manager.stream_ctx);
    avformat_close_input(&manager.ifmt_ctx);
    if (manager.ofmt_ctx && !(manager.ofmt_ctx->oformat->flags & AVFMT_NOFILE))
        avio_closep(&manager.ofmt_ctx->pb);
    avformat_free_context(manager.ofmt_ctx);
    
    if (ret < 0)
        av_log(NULL, AV_LOG_ERROR, "Error occurred: %s\n", av_err2str(ret));
    if (callBack != NULL) {
        callBack(inputPath,100);
    }
    return ret ? 1 : 0;
}


void managerDealProgress(TranscodeManager *manager, bool isFinish) {
    float progress = 0;
    if (isFinish) {
        progress = 100;
        manager->finished = true;
    }else {
        if (manager->compressed_frame != 0 && manager->total_frame != 0) {
            progress = manager->compressed_frame / (float)manager->total_frame;
        }
        progress = progress * 99;
    }
    if (manager->callBack) {
        (*manager->callBack)(manager->fileName,progress);
    }
    if (isFinish) {
        manager->callBack = NULL;
    }
    
}




VideoInfo *getMp4Info(char * input) {
    AVFormatContext *pFormatCtx = NULL;
    
    // 打开文件并读取头部
    if (avformat_open_input(&pFormatCtx, input, NULL, NULL) != 0)
        return NULL; // 无法打开文件
    
    // 获取流信息
    if (avformat_find_stream_info(pFormatCtx, NULL) < 0)
        return NULL; // 无法获取流信息
    
    // 找到第一个视频流
    int videoStream = -1;
    for (int i = 0; i < pFormatCtx->nb_streams; i++)
        if (pFormatCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            videoStream = i;
            break;
        }
    
    if (videoStream == -1)
        return NULL; // 没有找到视频流
    
    // 获取编解码器上下文
    AVStream *avStream = pFormatCtx->streams[videoStream];
    AVCodecParameters *codecInfo = pFormatCtx->streams[videoStream]->codecpar;
    VideoInfo *info = (VideoInfo *)malloc(sizeof(VideoInfo));
    info->width = codecInfo->width;
    info->height = codecInfo->height;
    info->bitrate = codecInfo->bit_rate;
    info->duration = avStream->duration * av_q2d(avStream->time_base);
    // 打印视频信息
    // 关闭文件
    avformat_close_input(&pFormatCtx);
    return info;
}

