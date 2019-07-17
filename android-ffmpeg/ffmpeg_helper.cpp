//
// Created by MOMO on 2019-07-15.
//

#include <iostream>
#include "ffmpeg_helper.h"

using namespace std;

FFmpegHelper::FFmpegHelper(FFmpegConfig config) {
    this->config = config;
    codec = nullptr;
    codec_context = nullptr;
    frame_count = 0;

}

FFmpegHelper::~FFmpegHelper() {
    avcodec_free_context(&codec_context);
    avformat_free_context(formatContext);
    av_frame_free(&frame);
}

void FFmpegHelper::init() {
    status = STATUS_START;
    ret = avformat_alloc_output_context2(&formatContext, nullptr, nullptr, config.file_name);
    if (ret < 0) {
        cout << "avformat_alloc_output_context2" << endl;
        //TODO
        return;
    }

    codec = avcodec_find_encoder(formatContext->oformat->video_codec);
    if (!codec) {
        cout << "avcodec_find_encoder" << endl;
        //TODO
        return;
    }
    codec_context = avcodec_alloc_context3(codec);
    if (!codec_context) {
        cout << "avcodec_alloc_context3" << endl;

        //TODO
        return;
    }
    codec_context->codec_id = formatContext->oformat->video_codec;
    codec_context->bit_rate = config.encoding_bit_rate;
    codec_context->width = config.width;
    codec_context->height = config.height;
    codec_context->pix_fmt = AV_PIX_FMT_YUV420P;// TODO
    codec_context->gop_size = config.i_frame_interval;
    codec_context->framerate = AVRational{config.frame_rate, 1};
    codec_context->time_base = AVRational{1, config.frame_rate};

    if (formatContext->oformat->flags & AVFMT_GLOBALHEADER)
        codec_context->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;

    ret = avcodec_open2(codec_context, codec, nullptr);
    if (ret < 0) {
        cout << "avcodec_open2" << endl;
        //TODO
        return;
    }
    frame = av_frame_alloc();
    frame->height = config.height;
    frame->width = config.width;
    frame->format = codec_context->pix_fmt;
    ret = av_frame_get_buffer(frame, 0);
    if (ret < 0) {
        cout << "av_frame_get_buffer" << endl;
        //TODO
        return;
    }

    ret = avio_open(&formatContext->pb, config.file_name, AVIO_FLAG_WRITE);
    if (ret < 0) {
        cout << "avio_open" << endl;
        //TODO
        return;
    }
    stream = avformat_new_stream(formatContext, codec);
    stream->id = formatContext->nb_streams - 1;
    stream->time_base = codec_context->time_base;

    ret = avcodec_parameters_from_context(stream->codecpar, codec_context);
    if (ret < 0) {
        cout << "avcodec_parameters_from_context" << endl;
        return;
    }
    if (!stream) {
        cout << "avformat_new_stream" << endl;
        return;
    }
    av_dump_format(formatContext, 0, config.file_name, 1);
    ret = avformat_write_header(formatContext, nullptr);
    if (ret < 0) {
        cout << "avformat_write_header" << endl;
        //TODO
        return;
    }
}

void FFmpegHelper::encode(uint8_t *data_y, uint8_t *data_u, uint8_t *data_v) {
    ret = av_frame_make_writable(frame);
    if (ret < 0) {
        cout << "av_frame_make_writable" << endl;
        //TODO
        return;
    }

    int x, y, i;
    int height = config.height;
    int width = config.width;
    i = frame_count;
    for (y = 0; y < height; y++)
        for (x = 0; x < width; x++)
            frame->data[0][y * frame->linesize[0] + x] = static_cast<uint8_t>(x + y + i * 3);

    /* Cb and Cr */
    for (y = 0; y < height / 2; y++) {
        for (x = 0; x < width / 2; x++) {
            frame->data[1][y * frame->linesize[1] + x] = static_cast<uint8_t>(128 + y + i * 2);
            frame->data[2][y * frame->linesize[2] + x] = static_cast<uint8_t>(64 + x + i * 5);
        }
    }


//    frame->data[0] = data_y;
//    frame->data[1] = data_u;
//    frame->data[2] = data_v;
    frame->pts = frame_count++;

    av_init_packet(&packet);
    int got_packet;
    ret = avcodec_encode_video2(codec_context, &packet, frame, &got_packet);
    if (ret < 0) {
        fprintf(stderr, "Error encoding video frame: %s\n", av_err2str(ret));
        exit(1);
    }

    if (got_packet) {
        printf("ssssssss\n");
        av_packet_rescale_ts(&packet, codec_context->time_base, stream->time_base);
        packet.stream_index = stream->index;
        ret = av_interleaved_write_frame(formatContext, &packet);
    } else {
        printf("cccccccc\n");
        ret = 0;
    }

//    av_init_packet(packet);
//    avcodec_send_frame(codec_context, frame);
//    ret = avcodec_receive_packet(codec_context, packet);
    if (ret == EAGAIN) {

    } else if (ret == EINVAL) {

    } else if (ret == AVERROR_EOF) {

    } else if (ret < 0) {

    }
//
//    av_packet_unref(packet);
//    ret = av_write_frame(formatContext, packet);
    if (ret < 0) {
        cout << "av_write_frame:" << av_err2str(ret) << endl;
        //TODO
    }
}

Status FFmpegHelper::getStatus() {
    return status;
}

void FFmpegHelper::release() {
    status = STATUS_RELEASE;
    av_write_trailer(formatContext);
    avio_close(formatContext->pb);
}



