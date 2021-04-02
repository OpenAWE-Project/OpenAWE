//
// Created by patrick on 25.05.20.
//

#ifndef AWE_THEORA_H
#define AWE_THEORA_H

#include <memory>
#include <mutex>

#include <ogg/ogg.h>
#include <theora/theoradec.h>
#include <vorbis/codec.h>

#include "src/common/readstream.h"
#include "src/common/memwritestream.h"

#include "src/video/codecs/codec.h"

#include "src/sound/types.h"

namespace Video {

class Theora : public Codec {
public:
	Theora(Common::ReadStream *);
	~Theora();

	Graphics::Surface * getNextFrame() override;

	void getNextAudio(Sound::Buffer &buffer, unsigned int stream) override;

private:
	struct VideoStream {
		ogg_stream_state _videoStream;
		th_dec_ctx *_videoDecoder;
	};

	struct AudioStream {
		ogg_stream_state audioStream;
		vorbis_dsp_state audioDecoder;
		vorbis_block audioBlock;
		vorbis_info info;
	};

	void decodeYUV420(th_ycbcr_buffer, Graphics::Surface *);

	void queuePage(ogg_page *page);
	void bufferData();

	std::unique_ptr<Common::ReadStream> _file;

	unsigned int _picX, _picY;

	ogg_sync_state _sync;

	ogg_stream_state _videoStream;
	th_dec_ctx *_videoDecoder;

	std::vector<AudioStream> _audioStreams;

	th_pixel_fmt _pixelFormat;
};

}

#endif //AWE_THEORA_H
