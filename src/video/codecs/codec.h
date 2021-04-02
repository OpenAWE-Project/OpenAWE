//
// Created by patrick on 25.05.20.
//

#ifndef AWE_CODEC_H
#define AWE_CODEC_H

#include "src/graphics/images/surface.h"

#include "src/sound/buffer.h"
#include "src/sound/types.h"

namespace Video {

class Codec {
public:
	Codec();

	float getFps() const;
	void getSize(unsigned int &width, unsigned int &height) const;

	bool eos() const;

	virtual Graphics::Surface * getNextFrame() = 0;
	virtual void getNextAudio(Sound::Buffer &buffer, unsigned int stream) = 0;

protected:
	struct AudioInfo {
		Sound::Format channelCount;
		unsigned int sampleRate;
	};

	float _fps;
	bool _eos;

	unsigned int _width, _height;
};

} // End of namespace Video

#endif //AWE_CODEC_H
