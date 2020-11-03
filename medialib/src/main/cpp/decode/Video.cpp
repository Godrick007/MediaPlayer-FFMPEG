//
// Created by Godrick Crown on 2020/11/2.
//

#include "Video.h"

Video::Video(PlayState *playState, Callback2Java *cb2j) {
    this->playState = playState;
    this->cb2j = cb2j;
}

Video::~Video() {

}

void Video::play() {

}
