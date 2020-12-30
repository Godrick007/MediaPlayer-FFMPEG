//
// Created by Godrick Crown on 2020/10/31.
//

#ifndef MEDIAPLAYER_FFMPEG_PLAYSTATE_H
#define MEDIAPLAYER_FFMPEG_PLAYSTATE_H


class PlayState {
public:
    static const int JNI_ERROR_ON_LOAD = 1001;
    static const int JNI_ERROR_REGISTER_METHOD = 1002;

    static const int MP_INIT_URL_ERROR = 2001;
    static const int MP_INIT_STREAM_ERROR = 2002;
    static const int MP_INIT_FIND_CODEC_ERROR = 2003;
    static const int MP_INIT_ALLOC_CODEC_ERROR = 2004;
    static const int MP_INIT_FILL_CODEC_ERROR = 2005;
    static const int MP_INIT_OPEN_CODEC_ERROR = 2006;


public:

    static const int THRESHOLD_SLEEP_10 = 1000 * 10;
    static const int THRESHOLD_SLEEP_100 = 1000 * 100;
    static const int THRESHOLD_SLEEP_500 = 1000 * 500;
    static const int THRESHOLD_QUEUE_LIMIT = 40;

public:
    bool exit;
    bool load = true;
    bool seek = false;
    bool pause = false;
    bool record = false;
public:
    PlayState();

    ~PlayState();


};


#endif //MEDIAPLAYER_FFMPEG_PLAYSTATE_H
