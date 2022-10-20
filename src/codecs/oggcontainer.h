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

#ifndef OPENAWE_OGGCONTAINER_H
#define OPENAWE_OGGCONTAINER_H

#include <memory>
#include <optional>

#include <ogg/ogg.h>

#include "src/common/readstream.h"

#include "src/codecs/container.h"

namespace Codecs {

class OggContainer : public Container {
public:
	OggContainer(Common::ReadStream *stream);
	~OggContainer();

	size_t getNumAudioStreams() const override;

	bool hasVideoStream() const override;

	AudioStream *createAudioStream(size_t index) override;

	void readNextPage();

	VideoStream *createVideoStream() override;

private:
	void bufferData();
	void queuePage(ogg_page &page);

	std::vector<ogg_stream_state> _audioStreams;
	std::optional<ogg_stream_state> _videoStream;

	std::unique_ptr<Common::ReadStream> _stream;

	ogg_sync_state _sync;
};

class OggStream {
protected:
	OggStream(ogg_stream_state &stream, OggContainer &container);

	void readNextPacket(ogg_packet &packet);
	bool eos() const;

private:
	OggContainer &_container;
	ogg_stream_state &_stream;
};

} // End of namespace Codecs

#endif //OPENAWE_OGGCONTAINER_H
