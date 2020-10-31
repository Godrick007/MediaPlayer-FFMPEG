//
// Created by Godrick Crown on 2020/10/31.
//

#ifndef MEDIAPLAYER_FFMPEG_PLAYSTATE_H
#define MEDIAPLAYER_FFMPEG_PLAYSTATE_H


class PlayState {
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
