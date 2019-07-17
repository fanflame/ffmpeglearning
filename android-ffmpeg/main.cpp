//
// Created by fanyiran on 2019-07-5.
//

#include "ffmpeg_status.h"
#include "ffmpeg_helper.h"

const char *TAG = "ffmpegtag";

FFmpegHelper *fFmpegHelper;

void startRecord();

void release();

void encode(uint8_t *y, uint8_t *u, uint8_t *data_v);

int main(int argc, char **argv) {
    FFmpegConfig fFmpegConfig;
    fFmpegConfig.width = 352;
    fFmpegConfig.height = 288;
    fFmpegConfig.frame_rate = 25;
    fFmpegConfig.encoding_bit_rate = 400000;
    fFmpegConfig.i_frame_interval = 12;
    fFmpegConfig.file_name = "/Users/momo/Documents/CLionProjects/ffmpeg/testvideos/encode.mp4";
    fFmpegHelper = new FFmpegHelper(fFmpegConfig);

    startRecord();
    int x, y;
    int height = fFmpegConfig.height;
    int width = fFmpegConfig.width;

    uint8_t *data_y = new uint8_t[height * width];
    uint8_t *data_u = new uint8_t[height * width / 4];
    uint8_t *data_v = new uint8_t[height * width / 4];

    for (int i = 0; i < 400; ++i) {
        encode(data_y,data_u,data_v);
    }
    release();

    return 0;
}

void startRecord() {
    if (!fFmpegHelper) {
        return;
    }
    fFmpegHelper->init();
}

void encode(uint8_t *data_y, uint8_t *data_u, uint8_t *data_v) {
    if (!fFmpegHelper || fFmpegHelper->getStatus() != STATUS_START) {
        return;
    }
    fFmpegHelper->encode(data_y, data_u, data_v);
}

void release() {
    if (!fFmpegHelper) {
        return;
    }
    fFmpegHelper->release();
    delete (fFmpegHelper);
    fFmpegHelper = NULL;
}