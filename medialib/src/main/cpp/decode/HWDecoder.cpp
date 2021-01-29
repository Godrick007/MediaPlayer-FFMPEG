//
// Created by Godrick Crown on 2021/1/28.
//

#include "HWDecoder.h"

void HWDecoder::init(const char *mimeType, int width, int height, void *csd0, void *csd1,
                     size_t csd0_size, size_t csd1_size,
                     ANativeWindow *surface) {

    format = AMediaFormat_new();
    AMediaFormat_setInt32(format, AMEDIAFORMAT_KEY_MAX_INPUT_SIZE, width * height);
    AMediaFormat_setBuffer(format, AMEDIAFORMAT_KEY_CSD_0, csd0, csd0_size);
    AMediaFormat_setBuffer(format, AMEDIAFORMAT_KEY_CSD_1, csd1, csd1_size);
    codec = AMediaCodec_createDecoderByType(mimeType);
    AMediaCodec_dequeueOutputBuffer(codec, outputBuffInfo, 10);
    AMediaCodec_configure(codec, format, surface, nullptr, 0);
    AMediaCodec_start(codec);

}

HWDecoder::HWDecoder() {

}

HWDecoder::~HWDecoder() {

}
