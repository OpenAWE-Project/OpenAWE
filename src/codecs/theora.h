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

#ifndef OPENAWE_THEORA_H
#define OPENAWE_THEORA_H

#include <theora/theoradec.h>

#include "src/codecs/videostream.h"
#include "src/codecs/oggcontainer.h"

namespace Codecs {

class TheoraStream : public VideoStream, protected OggStream {
public:
	TheoraStream(
		const th_info &info,
		th_setup_info *setupInfo,
		ogg_stream_state &stream,
		OggContainer &container
	);
	~TheoraStream();

	void readNextFrame(YCbCrBuffer &ycbcrBuffer) override;

	bool eos() const override;

	void skipNextFrames(size_t numFrames) override;

private:
	th_dec_ctx *_decoder;
};

} // End of namespace Codecs

#endif //OPENAWE_THEORA_H
