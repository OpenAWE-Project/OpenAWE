//
// Created by patrick on 25.09.22.
//

#include <cstring>
#include "vorbis.h"
#include "src/common/exception.h"

namespace Codecs {

VorbisStream::VorbisStream(vorbis_info info, ogg_stream_state &stream, OggContainer &container) :
	OggStream(
		stream,
		container
	),
	AudioStream(
		info.rate,
		16,
		info.channels,
		true
	),
	_pos(0),
	_info(info) {
	if (vorbis_synthesis_init(&_dsp, &_info) != 0)
		throw CreateException("Failed to create synthesis object");
	if (vorbis_block_init(&_dsp, &_block) != 0)
		throw CreateException("Failed to create vorbis block");
}

VorbisStream::~VorbisStream() {
	vorbis_block_clear(&_block);
	vorbis_dsp_clear(&_dsp);
	vorbis_info_clear(&_info);
}

bool VorbisStream::eos() const {
	return OggStream::eos();
}

size_t VorbisStream::pos() const {
	return _pos;
}

std::vector<byte> VorbisStream::read(size_t numSamples) {
	ogg_packet packet;
	float  **pcm;
	size_t remainingSamples = numSamples;
	std::vector<int16_t> pcmData;
	do {
		readNextPacket(packet);
		vorbis_synthesis(&_block, &packet);
		vorbis_synthesis_blockin(&_dsp, &_block);
		size_t readSamples = vorbis_synthesis_pcmout(&_dsp, &pcm);
		for (int i = 0; i < std::min(readSamples, remainingSamples); ++i) {
			for (int j = 0; j < getChannelCount(); ++j) {
				pcmData.emplace_back(pcm[j][i] * std::numeric_limits<int16_t>::max());
			}
		}

		vorbis_synthesis_read(&_dsp, static_cast<int>(readSamples));
		remainingSamples -= readSamples;
		_pos += readSamples;
	} while (remainingSamples <= 0);

	std::vector<byte> rawPcmData;
	rawPcmData.resize(pcmData.size() * sizeof(int16_t));
	std::memcpy(rawPcmData.data(), pcmData.data(), pcmData.size() * sizeof(int16_t));

	return rawPcmData;
}

}
