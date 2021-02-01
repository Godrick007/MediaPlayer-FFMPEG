//
// Created by Godrick Crown on 2021/1/28.
//

#ifndef MEDIAPLAYER_FFMPEG_HWDECODER_H
#define MEDIAPLAYER_FFMPEG_HWDECODER_H

#include <cstring>

#include <media/NdkMediaCodec.h>
#include <media/NdkMediaFormat.h>
#include <media/NdkMediaExtractor.h>

class HWDecoder {

public:

    AMediaCodec *codec = nullptr;
    AMediaFormat *format = nullptr;
    AMediaExtractor *extractor = nullptr;
    AMediaCodecBufferInfo *outputBuffInfo = nullptr;

    ANativeWindow *surface = nullptr;


public:

    HWDecoder();

    ~HWDecoder();

    void init(const char *mimeType, int width, int height, void *csd0, void *csd1, size_t csd0_size,
              size_t csd1_size,
              ANativeWindow *surface);

    void decode(void *data, int size);


};


#endif //MEDIAPLAYER_FFMPEG_HWDECODER_H
