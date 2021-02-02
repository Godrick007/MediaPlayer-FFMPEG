//
// Created by Godrick Crown on 2021/1/28.
//


#include "HWDecoder.h"

void HWDecoder::init(const char *mimeType, int width, int height, void *csd0, void *csd1,
                     size_t csd0_size, size_t csd1_size,
                     ANativeWindow *surface) {

    format = AMediaFormat_new();
    AMediaFormat_setInt32(format, AMEDIAFORMAT_KEY_MAX_INPUT_SIZE, width * height);
    AMediaFormat_setBuffer(format, "csd_0", csd0, csd0_size);
    AMediaFormat_setBuffer(format, "csd_1", csd1, csd1_size);
    codec = AMediaCodec_createDecoderByType(mimeType);
    AMediaCodec_dequeueOutputBuffer(codec, outputBuffInfo, 10);
    AMediaCodec_configure(codec, format, surface, nullptr, 0);
    AMediaCodec_start(codec);

    this->surface = surface;

}

HWDecoder::HWDecoder() {

}

HWDecoder::~HWDecoder() {

}

void HWDecoder::decode(void *data, int size) {

    if (surface != nullptr && data != nullptr && codec != nullptr && size > 0) {

        int inputBufferIndex = AMediaCodec_dequeueInputBuffer(codec, 10);

        if (inputBufferIndex > 0) {

            size_t bufferSize;

            uint8_t *buf = AMediaCodec_getInputBuffer(codec, inputBufferIndex, &bufferSize);

            memset(buf, 0, size);

            memcpy(buf, data, size);

            AMediaCodec_queueInputBuffer(codec, inputBufferIndex, 0, size, 0, 0);

        }

        int outputBufferIndex = AMediaCodec_dequeueOutputBuffer(codec, outputBuffInfo, 10);

        while (outputBufferIndex > 0) {

            AMediaCodec_releaseOutputBuffer(codec, outputBufferIndex, true);

            outputBufferIndex = AMediaCodec_dequeueOutputBuffer(codec, outputBuffInfo, 10);

        }


    }

}
