/* OpenAWE - A reimplementation of Remedys Alan Wake Engine
 *
 * OpenAWE is the legal property of its developers, whose names
 * can be found in the AUTHORS file distributed with this source
 * distribution.
 *
 * OpenAWE is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 3
 * of the License, or (at your option) any later version.
 *
 * OpenAWE is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with OpenAWE. If not, see <http://www.gnu.org/licenses/>.
 */

#include <vorbis/codec.h>
#include <theora/theoradec.h>
#include <spdlog/spdlog.h>

#include "src/common/exception.h"

#include "src/codecs/oggcontainer.h"
#include "vorbis.h"
#include "theora.h"

namespace Codecs {

OggContainer::OggContainer(Common::ReadStream *stream) : _stream(stream) {
	ogg_sync_init(&_sync);

	ogg_page page;
	ogg_packet packet;
	do {
		bufferData();

		while(ogg_sync_pageout(&_sync, &page)) {
			if (!ogg_page_bos(&page))
				break;

			ogg_stream_state test;
			ogg_stream_init(&test, ogg_page_serialno(&page));
			ogg_stream_pagein(&test, &page);
			ogg_stream_packetpeek(&test, &packet);

			if (vorbis_synthesis_idheader(&packet)) {
				_audioStreams.emplace_back();
				std::memcpy(&_audioStreams.back(), &test, sizeof(ogg_stream_state));
			} else if (th_packet_isheader(&packet)) {
				_videoStream = ogg_stream_state{};
				std::memcpy(&*_videoStream, &test, sizeof(ogg_stream_state));
			} else {
				spdlog::warn("Unknown stream found with serial number {}", ogg_page_serialno(&page));
				ogg_stream_destroy(&test);
			}
		}
	} while (ogg_page_bos(&page));

	queuePage(page);
}

OggContainer::~OggContainer() {
	if (_videoStream)
		ogg_stream_clear(&*_videoStream);

	for (auto &stream: _audioStreams) {
		ogg_stream_clear(&stream);
	}

	ogg_sync_destroy(&_sync);
}

void OggContainer::readNextPage() {
	ogg_page page;
	while (ogg_sync_pageout(&_sync, &page) != 1) {
		bufferData();
	}

	queuePage(page);
}

void OggContainer::bufferData() {
	char *buffer = ogg_sync_buffer(&_sync, 8192);
	size_t readBytes = _stream->read(buffer, 8192);
	ogg_sync_wrote(&_sync, static_cast<long>(readBytes));
}

void OggContainer::queuePage(ogg_page &page) {
	if (_videoStream)
		if (ogg_stream_pagein(&*_videoStream, &page) == 0)
			return;

	for (auto &stream : _audioStreams) {
		if (ogg_stream_pagein(&stream, &page) == 0)
			return;
	}
}

AudioStream *OggContainer::createAudioStream(size_t index) {
	if (index >= _audioStreams.size())
		throw CreateException("Invalid audio stream index given, expected index less than {}", _audioStreams.size());

	ogg_stream_state &newVorbisState = _audioStreams[index];

	vorbis_info info;
	vorbis_comment comments;

	vorbis_info_init(&info);
	vorbis_comment_init(&comments);

	ogg_packet packet;
	for (int i = 0; i < 3; ++i) {
		while (ogg_stream_packetout(&newVorbisState, &packet) != 1) {
			readNextPage();
		}

		vorbis_synthesis_headerin(&info, &comments, &packet);
	}

	auto vorbis = new VorbisStream(info, newVorbisState, *this);

	vorbis_comment_clear(&comments);

	return vorbis;
}

VideoStream *OggContainer::createVideoStream() {
	if (!_videoStream)
		throw CreateException("Ogg container doesnt contain a video stream");

	ogg_stream_state &newTheoraState = *_videoStream;


	th_info info;
	th_comment comments;
	th_setup_info *theoraSetupInfo = nullptr;

	th_info_init(&info);
	th_comment_init(&comments);

	ogg_packet packet;

	int headerState = 0;
	do {
		int ret = 0;
		while ((ret = ogg_stream_packetout(&newTheoraState, &packet)) != 1) {
			if (ret == 0)
				readNextPage();
		}

		headerState = th_decode_headerin(&info, &comments, &theoraSetupInfo, &packet);
		if (headerState < 0)
			throw CreateException("Error while parsing theora header: {}", headerState);
	} while(headerState != 1);

	auto theora = new TheoraStream(info, theoraSetupInfo, newTheoraState, *this);

	th_info_clear(&info);
	th_comment_clear(&comments);
	th_setup_free(theoraSetupInfo);

	return theora;
}

size_t OggContainer::getNumAudioStreams() const {
	return _audioStreams.size();
}

bool OggContainer::hasVideoStream() const {
	return _videoStream.has_value();
}

OggStream::OggStream(ogg_stream_state &stream, OggContainer &container) : _stream(stream), _container(container) {
}

void OggStream::readNextPacket(ogg_packet &packet) {
	if (ogg_stream_eos(&_stream))
		return;

	while (ogg_stream_packetout(&_stream, &packet) != 1) {
		_container.readNextPage();
	}
}

bool OggStream::eos() const {
	return ogg_stream_eos(&_stream);
}

} // End of namespace Codecs