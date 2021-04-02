//
// Created by patrick on 25.05.20.
//

#include "codec.h"

namespace Video {

Codec::Codec() : _eos(false) {

}

bool Codec::eos() const {
	return _eos;
}

float Codec::getFps() const {
	return _fps;
}

void Codec::getSize(unsigned int &width, unsigned int &height) const {
	width = _width;
	height = _height;
}

}
