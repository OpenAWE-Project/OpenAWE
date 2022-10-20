//
// Created by patrick on 25.09.22.
//

#ifndef OPENAWE_VORBIS_H
#define OPENAWE_VORBIS_H

#include <ogg/ogg.h>
#include <vorbis/codec.h>

#include "src/codecs/audiostream.h"
#include "src/codecs/oggcontainer.h"

namespace Codecs {

class VorbisStream : public AudioStream, protected OggStream {
public:
	VorbisStream(vorbis_info info, ogg_stream_state &stream, OggContainer &container);

	virtual ~VorbisStream();

	bool eos() const override;

	std::vector<byte> read(size_t numSamples) override;

	size_t pos() const override;

private:
	vorbis_info _info;
	vorbis_dsp_state _dsp;
	vorbis_block _block;
	size_t _pos;
};

} // End of namespace Codecs

#endif //OPENAWE_VORBIS_H
