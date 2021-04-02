//
// Created by patrick on 19.04.20.
//

#ifndef AWE_PLAYER_H
#define AWE_PLAYER_H

#include <memory>
#include <future>
#include <queue>
#include <chrono>
#include <vector>

#include "src/common/uuid.h"

#include "src/graphics/images/surface.h"

#include "src/sound/stream.h"

#include "src/video/codecs/codec.h"

namespace Video {

class Player {
public:
	Player(unsigned int bufferSize = 20);

	void addAudioTrack(unsigned int id);

	void load(const std::string &videoFile);
	void play();

private:
	struct TextureToProcess {
		std::future<Common::UUID> textureFuture;
		std::unique_ptr<Graphics::Surface> surface;
	};

	struct AudioStream {
		unsigned int id;
		std::unique_ptr<Sound::Stream> stream;
		std::queue<Sound::Buffer> buffers;
	};

	void playLoop();
	void preloadLoop();
	void preloadAudioLoop();

	const unsigned int _bufferSize;

	unsigned int _width, _height;
	unsigned int _videoStreamIndex;

	std::unique_ptr<Codec> _codec;

	Common::UUID _currentFrame;
	std::mutex _audioStreamsMutex;

	std::chrono::system_clock::duration _frameDuration;
	std::chrono::system_clock::time_point _lastFrame;

	std::atomic_bool _pause, _stop;

	std::queue<TextureToProcess> _texturesToProcess;
	std::queue<Common::UUID> _texturesToDisplay;

	std::vector<std::unique_ptr<AudioStream>> _audio;

	std::queue<Sound::Buffer> _buffers;

	/*AVFormatContext *_formatCtx;
	AVCodecContext *_videoCtx;
	AVCodec *_videoCodec;

	struct AudioStream {
		AVCodecContext *audioCtx;
		AVCodec *audioCodec;
		unsigned int streamIndex;
		Sound::Stream *stream;
	};

	std::vector<AudioStream> _audioCtx;
	SwrContext *_resampleContext;

	AVFrame *_frame;
	AVPacket *_packet;*/
};

}

#endif //AWE_PLAYER_H
