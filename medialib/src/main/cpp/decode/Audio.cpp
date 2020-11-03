//
// Created by Godrick Crown on 2020/11/2.
//

#include "Audio.h"

Audio::Audio(PlayState *playState, int sampleRate, Callback2Java *cb2j) {
    this->playState = playState;
    this->sampleRate = sampleRate;
    this->cb2j = cb2j;
}

void Audio::play() {

}
