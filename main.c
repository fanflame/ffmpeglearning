/**
 * @file
 * libavformat API example.
 *
 * Output a media file in any supported libavformat format. The default
 * codecs are used.
 * @example muxing.c
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include <libavutil/avassert.h>
#include <libavutil/channel_layout.h>
#include <libavutil/opt.h>
#include <libavutil/mathematics.h>
#include <libavutil/timestamp.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libswresample/swresample.h>

#define STREAM_DURATION   10.0
#define STREAM_FRAME_RATE 25 /* 25 images/s */
#define STREAM_PIX_FMT    AV_PIX_FMT_YUV420P /* default pix_fmt */

// a wrapper around a single output AVStream
typedef struct OutputStream {
    AVStream *stream;
    AVCodecContext *avCodecContext;

    /* pts of the next frame that will be generated */
    int64_t next_pts;

    AVFrame *frame;

    struct SwsContext *sws_ctx;
    struct SwrContext *swr_ctx;
} OutputStream;

static void log_packet(const AVFormatContext *fmt_ctx, const AVPacket *pkt) {
    AVRational *time_base = &fmt_ctx->streams[pkt->stream_index]->time_base;

    printf("pts:%s pts_time:%s dts:%s dts_time:%s duration:%s duration_time:%s stream_index:%d\n",
           av_ts2str(pkt->pts), av_ts2timestr(pkt->pts, time_base),
           av_ts2str(pkt->dts), av_ts2timestr(pkt->dts, time_base),
           av_ts2str(pkt->duration), av_ts2timestr(pkt->duration, time_base),
           pkt->stream_index);
}

static int write_frame(AVFormatContext *avFormatContext, const AVRational *time_base, AVStream *st, AVPacket *pkt) {
    /* rescale output packet timestamp values from codec to stream timebase */
    av_packet_rescale_ts(pkt, *time_base, st->time_base);
    pkt->stream_index = st->index;

    /* Write the compressed frame to the media file. */
    log_packet(avFormatContext, pkt);
    return av_interleaved_write_frame(avFormatContext, pkt);
}

static void add_stream(OutputStream *outputStream, AVFormatContext *formatContext,
                       AVCodec **codec,
                       enum AVCodecID codec_id) {
    AVCodecContext *avCodecContext;

    /* 根据code_id查找注册过的encoder */
    *codec = avcodec_find_encoder(codec_id);
    if (!(*codec)) {
        fprintf(stderr, "Could not find encoder for '%s'\n",
                avcodec_get_name(codec_id));
        exit(1);
    }

    /*给formatcontext添加一个stream*/
    outputStream->stream = avformat_new_stream(formatContext, NULL);
    if (!outputStream->stream) {
        fprintf(stderr, "Could not allocate stream\n");
        exit(1);
    }
    outputStream->stream->id = formatContext->nb_streams - 1;
    /*初始化AVCodecContext并设置默认值*/
    avCodecContext = avcodec_alloc_context3(*codec);
    if (!avCodecContext) {
        fprintf(stderr, "Could not alloc an encoding context\n");
        exit(1);
    }
    outputStream->avCodecContext = avCodecContext;

    avCodecContext->codec_id = codec_id;

    avCodecContext->bit_rate = 400000;
    /* Resolution must be a multiple of two. */
    avCodecContext->width = 352;
    avCodecContext->height = 288;
    /* timebase: This is the fundamental unit of time (in seconds) in terms
     * of which frame timestamps are represented. For fixed-fps content,
     * timebase should be 1/framerate and timestamp increments should be
     * identical to 1. */
    outputStream->stream->time_base = (AVRational) {1, STREAM_FRAME_RATE};
    avCodecContext->time_base = outputStream->stream->time_base;

    avCodecContext->gop_size = 12; /* emit one intra frame every twelve frames at most */
    avCodecContext->pix_fmt = STREAM_PIX_FMT;
    if (avCodecContext->codec_id == AV_CODEC_ID_MPEG2VIDEO) {
        /* just for testing, we also add B-frames */
        avCodecContext->max_b_frames = 2;
    }
    if (avCodecContext->codec_id == AV_CODEC_ID_MPEG1VIDEO) {
        /* Needed to avoid using macroblocks in which some coeffs overflow.
         * This does not happen with normal video, it just happens here as
         * the motion of the chroma plane does not match the luma plane. */
        avCodecContext->mb_decision = 2;
    }

    /* Some formats want stream headers to be separate. */
    if (formatContext->oformat->flags & AVFMT_GLOBALHEADER)
        avCodecContext->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
}

/**************************************************************/

static AVFrame *alloc_picture(enum AVPixelFormat pix_fmt, int width, int height) {
    AVFrame *picture;
    int ret;

    /*初始化frame并设置默认值*/
    picture = av_frame_alloc();
    if (!picture)
        return NULL;

    picture->format = pix_fmt;
    picture->width = width;
    picture->height = height;

    /* 填充frame的data以及buf，如果有必要的话还会填充extended_data以及extended_buf */
    ret = av_frame_get_buffer(picture, 32);
    if (ret < 0) {
        fprintf(stderr, "Could not allocate frame data.\n");
        exit(1);
    }

    return picture;
}

static void open_video(AVCodec *codec, OutputStream *outputStream) {
    int ret;
    AVCodecContext *avCodecContext = outputStream->avCodecContext;

    /* open the codec */
    ret = avcodec_open2(avCodecContext, codec, NULL);
    if (ret < 0) {
        fprintf(stderr, "Could not open video codec: %s\n", av_err2str(ret));
        exit(1);
    }

    /* 分配并初始化一个可以重复使用的frame */
    outputStream->frame = alloc_picture(avCodecContext->pix_fmt, avCodecContext->width, avCodecContext->height);
    if (!outputStream->frame) {
        fprintf(stderr, "Could not allocate video frame\n");
        exit(1);
    }

    /* 将codecContext的stream参数拷贝到stream.codecpar*/
    ret = avcodec_parameters_from_context(outputStream->stream->codecpar, avCodecContext);
    if (ret < 0) {
        fprintf(stderr, "Could not copy the stream parameters\n");
        exit(1);
    }
}

/* Prepare a dummy image. */
static void fill_yuv_image(AVFrame *pict, int frame_index,
                           int width, int height) {
    int x, y, i;

    i = frame_index;

    /* Y */
    for (y = 0; y < height; y++)
        for (x = 0; x < width; x++)
            pict->data[0][y * pict->linesize[0] + x] = x + y + i * 3;

    /* Cb and Cr */
    for (y = 0; y < height / 2; y++) {
        for (x = 0; x < width / 2; x++) {
            pict->data[1][y * pict->linesize[1] + x] = 128 + y + i * 2;
            pict->data[2][y * pict->linesize[2] + x] = 64 + x + i * 5;
        }
    }
}

static AVFrame *get_video_frame(OutputStream *ost) {
    AVCodecContext *c = ost->avCodecContext;

    /* check if we want to generate more frames */
    if (av_compare_ts(ost->next_pts, c->time_base,
                      STREAM_DURATION, (AVRational) {1, 1}) >= 0)
        return NULL;

    /* when we pass a frame to the encoder, it may keep a reference to it
     * internally; make sure we do not overwrite it here */
    if (av_frame_make_writable(ost->frame) < 0)
        exit(1);

    fill_yuv_image(ost->frame, ost->next_pts, c->width, c->height);

    ost->frame->pts = ost->next_pts++;

    return ost->frame;
}

/*
 * encode one video frame and send it to the muxer
 * return 1 when encoding is finished, 0 otherwise
 */
static int write_video_frame(AVFormatContext *oc, OutputStream *outputStream) {
    int ret;
    AVCodecContext *avCodecContext;
    AVFrame *frame;
    int got_packet = 0;
    AVPacket pkt = {0};

    avCodecContext = outputStream->avCodecContext;

    frame = get_video_frame(outputStream);

    av_init_packet(&pkt);

    /* encode the image */
    ret = avcodec_encode_video2(avCodecContext, &pkt, frame, &got_packet);
    if (ret < 0) {
        fprintf(stderr, "Error encoding video frame: %s\n", av_err2str(ret));
        exit(1);
    }

    if (got_packet) {
        printf("ssssssss\n");
        ret = write_frame(oc, &avCodecContext->time_base, outputStream->stream, &pkt);
    } else {
        printf("cccccccc\n");
        ret = 0;
    }

    if (ret < 0) {
        fprintf(stderr, "Error while writing video frame: %s\n", av_err2str(ret));
        exit(1);
    }
    return (frame || got_packet) ? 0 : 1;
}

static void close_stream(AVFormatContext *oc, OutputStream *ost) {
    avcodec_free_context(&ost->avCodecContext);
    av_frame_free(&ost->frame);
    sws_freeContext(ost->sws_ctx);
    swr_free(&ost->swr_ctx);
}

int main(int argc, char **argv) {
    OutputStream outputStream = {0};
    const char *filename;
    AVOutputFormat *avOutputFormat;
    AVFormatContext *formatContext;
    AVCodec *avCodec;
    int ret;

    filename = "/Users/momo/Documents/CLionProjects/ffmpeg/testvideos/encode.mp4";

    /*初始化AVFormatContext*/
    avformat_alloc_output_context2(&formatContext, NULL, NULL, filename);
    if (!formatContext) {
        printf("Could not deduce output format from file extension: using MPEG.\n");
        avformat_alloc_output_context2(&formatContext, NULL, "mpeg", filename);
    }
    if (!formatContext)
        return 1;

    avOutputFormat = formatContext->oformat;

    /* Add the audio and video streams using the default format codecs
     * and initialize the codecs. */
    add_stream(&outputStream, formatContext, &avCodec, avOutputFormat->video_codec);

    /*打开视频codec，分配必要的编码buffers*/
    open_video(avCodec, &outputStream);

    /*打印输出格式的详细信息：
     * Output #0, mp4, to '/Users/momo/Documents/CLionProjects/ffmpeg/testvideos/encode.mp4':
     * Stream #0:0: Video: h264, yuv420p, 352x288, q=2-31, 400 kb/s, 25 tbn pts:0 pts_time:0 dts:-1*/
    av_dump_format(formatContext, 0, filename, 1);

    /*由filename指定创建和初始化AVIOContext，访问的资源*/
    ret = avio_open(&formatContext->pb, filename, AVIO_FLAG_WRITE);
    if (ret < 0) {
        fprintf(stderr, "Could not open '%s': %s\n", filename,
                av_err2str(ret));
        return 1;
    }

    /* Write the stream header, if any. */
    ret = avformat_write_header(formatContext, NULL);
    if (ret < 0) {
        fprintf(stderr, "Error occurred when opening output file: %s\n",
                av_err2str(ret));
        return 1;
    }

    int encode_video = 1;
    while (encode_video) {
        /* select the stream to encode */
        encode_video = !write_video_frame(formatContext, &outputStream);
    }

    /* Write the trailer, if any. The trailer must be written before you
     * close the CodecContexts open when you wrote the header; otherwise
     * av_write_trailer() may try to use memory that was freed on
     * av_codec_close(). */
    av_write_trailer(formatContext);

    /*释放资源*/
    close_stream(formatContext, &outputStream);

    if (!(avOutputFormat->flags & AVFMT_NOFILE))
        /*关闭输出文件*/
        avio_closep(&formatContext->pb);

    /*释放AVFormatContext*/
    avformat_free_context(formatContext);

    return 0;
}
