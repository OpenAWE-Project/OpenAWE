//
// Created by patrick on 25.05.20.
//

#include <algorithm>
#include <cstring>
#include <stdexcept>
#include <map>

#include <fmt/format.h>
#include <iostream>
#include <assert.h>

#include "src/common/writefile.h"
#include "src/common/memreadstream.h"

#include "src/graphics/images/dumptga.h"

#include "src/sound//dumpwav.h"

#include "src/video/codecs/theora.h"

struct AudioBootstrap {
	vorbis_info info;
	vorbis_comment  comment;
};

namespace Video {

Theora::Theora(Common::ReadStream *file) : _file(file) {
	ogg_sync_init(&_sync);

	// Vorbis bootstrap data
	std::map<long, AudioBootstrap> audioBootstrap;

	// Theora info structs
	th_comment theoraComment;
	th_info theoraInfo;
	th_setup_info *theoraSetupInfo = nullptr;

	th_comment_init(&theoraComment);
	th_info_init(&theoraInfo);

	ogg_page page;
	ogg_packet packet;

	// Parse initial headers
	bool headerDone = false;
	bool videoFound = false;
	while (!headerDone) {
		bufferData();

		while (ogg_sync_pageout(&_sync, &page)) {
			if (!ogg_page_bos(&page)) {
				queuePage(&page);
				headerDone = true;
				break;
			}

			ogg_stream_state test;
			ogg_stream_init(&test, ogg_page_serialno(&page));
			ogg_stream_pagein(&test, &page);
			ogg_stream_packetout(&test, &packet);

			if (th_packet_isheader(&packet) == 1 && !videoFound) {
				th_decode_headerin(&theoraInfo, &theoraComment, &theoraSetupInfo, &packet);
				std::memcpy(&_videoStream, &test, sizeof(ogg_stream_state));
				videoFound = true;
			} else if (vorbis_synthesis_idheader(&packet) == 1) {
				// Vorbis info structs
				vorbis_info vorbisInfo;
				vorbis_comment vorbisComment;

				vorbis_info_init(&vorbisInfo);
				vorbis_comment_init(&vorbisComment);

				vorbis_synthesis_headerin(&vorbisInfo, &vorbisComment, &packet);
				AudioStream audioStream{};
				AudioBootstrap audio{};

				std::memcpy(&audioStream.audioStream, &test, sizeof(ogg_stream_state));
				_audioStreams.push_back(audioStream);

				std::memcpy(&audio.info, &vorbisInfo, sizeof(vorbis_info));
				std::memcpy(&audio.comment, &vorbisComment, sizeof(vorbis_comment));
				audioBootstrap.insert(std::make_pair(audioStream.audioStream.serialno, audio));
			} else {
				ogg_stream_clear(&test);
			}
		}
	}

	if (!videoFound)
		throw std::runtime_error("No Video stream found in file");

	// Calculate fps
	_fps = static_cast<float>(theoraInfo.fps_numerator) / static_cast<float>(theoraInfo.fps_denominator);

	// Get width and height
	_width = theoraInfo.pic_width;
	_height = theoraInfo.pic_height;

	// Get the pixel format
	_pixelFormat = theoraInfo.pixel_fmt;

	// Get the picture offset
	_picX = theoraInfo.pic_x;
	_picY = theoraInfo.pic_y;

	// Parse additional theora headers
	bool theoraHeaderParsed = false;
	while (true) {
		while (ogg_stream_packetout(&_videoStream, &packet) > 0 && !theoraHeaderParsed) {
			int ret = th_decode_headerin(&theoraInfo, &theoraComment, &theoraSetupInfo, &packet);
			if (ret < 0)
				throw std::runtime_error("Invalid theora header");
			else if (ret == 1)
				theoraHeaderParsed = true;
		}

		if (theoraHeaderParsed)
			break;

		if (ogg_sync_pageout(&_sync, &page) > 0)
			queuePage(&page);
		else
			bufferData();
	}

	_videoDecoder = th_decode_alloc(&theoraInfo, theoraSetupInfo);

	int ppLevelMax = 0;
	th_decode_ctl(_videoDecoder, TH_DECCTL_GET_PPLEVEL_MAX, reinterpret_cast<void *>(&ppLevelMax), sizeof(int));
	th_decode_ctl(_videoDecoder, TH_DECCTL_SET_PPLEVEL, reinterpret_cast<void *>(&ppLevelMax), sizeof(int));

	// Cleaning up
	th_info_clear(&theoraInfo);
	th_comment_clear(&theoraComment);
	th_setup_free(theoraSetupInfo);

	// Parse additional vorbis headers
	for (auto &stream : _audioStreams) {
		bool vorbisHeaderParsed = false;
		unsigned int numVorbisHeadersParsed = 1;

		AudioBootstrap &bootstrap = audioBootstrap[stream.audioStream.serialno];

		while (true) {
			while (ogg_stream_packetout(&stream.audioStream, &packet) == 1 && !vorbisHeaderParsed) {
				int ret = vorbis_synthesis_headerin(&bootstrap.info, &bootstrap.comment, &packet);
				if (ret < 0)
					throw std::runtime_error("Invalid vorbis header");
				else if (ret == 0)
					numVorbisHeadersParsed += 1;

				if (numVorbisHeadersParsed >= 3)
					vorbisHeaderParsed = true;
			}

			if (vorbisHeaderParsed)
				break;

			if (ogg_sync_pageout(&_sync, &page) > 0)
				queuePage(&page);
			else
				bufferData();
		}
	}

	// Create vorbis dsp states and blocks
	for (auto &stream : _audioStreams) {
		AudioBootstrap &bootstrap = (audioBootstrap.find(stream.audioStream.serialno))->second;

		stream.info = bootstrap.info;

		vorbis_synthesis_init(&stream.audioDecoder, &stream.info);
		vorbis_block_init(&stream.audioDecoder, &stream.audioBlock);

		//vorbis_info_clear(&bootstrap.info);
		//vorbis_comment_clear(&bootstrap.comment);
	}
}

Theora::~Theora() {
	th_decode_free(_videoDecoder);
	ogg_stream_clear(&_videoStream);

	for (auto &item : _audioStreams) {
		vorbis_dsp_clear(&item.audioDecoder);
		vorbis_block_clear(&item.audioBlock);
		ogg_stream_clear(&item.audioStream);
	}
}

void Theora::queuePage(ogg_page *page) {
	if (ogg_stream_pagein(&_videoStream, page) == 0)
		return;

	for (auto &stream : _audioStreams) {
		if (ogg_stream_pagein(&stream.audioStream, page) == 0)
			return;
	}
}

void Theora::bufferData() {
	char *buffer = ogg_sync_buffer(&_sync, 8192);
	size_t readBytes = _file->read(buffer, 8192);
	//assert(readBytes > 0);
	ogg_sync_wrote(&_sync, readBytes);
}

Graphics::Surface * Theora::getNextFrame() {
	auto *frame = new Graphics::Surface(_width, _height, Graphics::ImageDecoder::RGB8);

	ogg_packet packet;
	while (ogg_stream_packetout(&_videoStream, &packet) != 1) {
		ogg_page page;
		while (ogg_sync_pageout(&_sync, &page) != 1) {
			bufferData();
		}

		queuePage(&page);

		if (ogg_page_eos(&page))
			_eos = true;
	}

	if (packet.granulepos)
		th_decode_ctl(_videoDecoder, TH_DECCTL_SET_GRANPOS, &packet.granulepos, sizeof(packet.granulepos));

	int ret = th_decode_packetin(_videoDecoder, &packet, nullptr);
	if (ret == 1)
		return frame;

	if (ret != 0)
		throw std::runtime_error("Error on decoding packet");

	th_ycbcr_buffer yuv;
	if (th_decode_ycbcr_out(_videoDecoder, yuv) != 0)
		throw std::runtime_error("Error decoding ycbcr");

	switch (_pixelFormat) {
		case TH_PF_420:
			decodeYUV420(yuv, frame);
			break;
		default:
			throw std::runtime_error("Invalid or unsupported pixel format");
	}

	return frame;
}

void Theora::getNextAudio(Sound::Buffer &buffer, unsigned int stream) {
	AudioStream &audio = _audioStreams[stream];

	ogg_packet packet;
	int ret, samplesRead=0;
	float  **pcm;
	do {
		while (ogg_stream_packetout(&audio.audioStream, &packet) != 1) {
			ogg_page page;
			while (ogg_sync_pageout(&_sync, &page) != 1) {
				bufferData();
			}
			queuePage(&page);
		}

		ret = vorbis_synthesis(&audio.audioBlock, &packet);
		if (ret < 0)
			throw std::runtime_error("Could not synthesize vorbis Block");
		else if (ret > 0)
			continue;

		ret = vorbis_synthesis_blockin(&audio.audioDecoder, &audio.audioBlock);
		if (ret < 0)
			throw std::runtime_error("Could not synthesize block");

		samplesRead = vorbis_synthesis_pcmout(&audio.audioDecoder, &pcm);
	} while (samplesRead == 0);

	vorbis_synthesis_read(&audio.audioDecoder, samplesRead);
	if (samplesRead <= 0)
		throw std::runtime_error("Invalid vorbis data");

	int16_t pcmDecoded[samplesRead];
	for (int i = 0; i < samplesRead; ++i) {
		//float p = pcm[0][i];
		//std::cout << p << std::endl;
		pcmDecoded[i] = pcm[0][i] * 32767.f;
		std::clamp<short>(pcmDecoded[i], -32768, 32767);
		//f.writeUint16LE(pcmDecoded[i]);
	}

	/*_w->write(pcmDecoded, samplesRead * sizeof(int16_t));
	Common::WriteFile file("test.wav");
	Sound::dumpWAV(file, _w->getData(), _w->getLength());
	file.close();*/

	buffer.bufferData(
			Sound::kFormatMono16,
			audio.audioDecoder.vi->rate,
			pcmDecoded,
			samplesRead * sizeof(int16_t)
	);
}

void Theora::decodeYUV420(th_img_plane *ycbcr, Graphics::Surface *surface) {
	// Conversion taken from:
	// http://hg.icculus.org/icculus/theoraplay/file/tip/theoraplay_cvtrgb.h

	auto *dst = reinterpret_cast<byte*>(surface->getData(0));
	const int ystride = ycbcr[0].stride;
	const int cbstride = ycbcr[1].stride;
	const int crstride = ycbcr[2].stride;
	const int yoff = (_picX & ~1) + ystride * (_picY & ~1);
	const int cboff = (_picX / 2) + (cbstride) * (_picY / 2);
	const byte *py = ycbcr[0].data + yoff;
	const byte *pcb = ycbcr[1].data + cboff;
	const byte *pcr = ycbcr[2].data + cboff;

	for (int posy = 0; posy < _height; posy++)
	{
		int posx, poshalfx;

		posx = 0;
		for (poshalfx = 0; poshalfx < (_width / 2); poshalfx++, posx += 2)
		{
			const byte y1 = py[posx];
			const byte y2 = py[posx + 1];
			const byte cb = pcb[poshalfx];
			const byte cr = pcr[poshalfx];

			*(dst++) = y1 + 1.402 * (cr - 128);
			*(dst++) = y1 - 0.34414 * (cb - 128) - 0.71414 * (cr - 128);
			*(dst++) = y1 + 1.772 * (cb - 128);

			*(dst++) = y2 + 1.402 * (cr - 128);
			*(dst++) = y2 - 0.34414 * (cb - 128) - 0.71414 * (cr - 128);
			*(dst++) = y2 + 1.772 * (cb - 128);
		}

		// adjust to the start of the next line.
		py += ystride;
		pcb += cbstride * (posy % 2);
		pcr += crstride * (posy % 2);
	}
}

}
