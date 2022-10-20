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

#include <cstring>

#include "theora.h"
#include "src/common/exception.h"

namespace Codecs {

TheoraStream::TheoraStream(
	const th_info &info,
	th_setup_info *setupInfo,
	ogg_stream_state &stream,
	OggContainer &container) :
	VideoStream(
		info.frame_width,
		info.frame_height,
		static_cast<float>(info.fps_numerator) / static_cast<float>(info.fps_denominator)
	),
	OggStream(
		stream,
		container
	) {
	if (info.pic_x != 0 || info.pic_y != 0)
		throw CreateException("Theora video offset is not supported!");

	_decoder = th_decode_alloc(&info, setupInfo);
	if (!_decoder)
		throw CreateException("Failure to create theora decoder");
}

TheoraStream::~TheoraStream() {
	th_decode_free(_decoder);
}

void TheoraStream::readNextFrame(YCbCrBuffer &ycbcrBuffer) {
	ogg_packet packet;
	readNextPacket(packet);

	if (packet.granulepos)
		th_decode_ctl(
			_decoder,
			TH_DECCTL_SET_GRANPOS,
			&packet.granulepos,
			sizeof(packet.granulepos)
		);

	th_decode_packetin(_decoder, &packet, nullptr);

	th_ycbcr_buffer buffer;
	th_decode_ycbcr_out(_decoder, buffer);

	if (ycbcrBuffer.y.size() != buffer[0].width * buffer[0].height)
		ycbcrBuffer.y.resize(buffer[0].width * buffer[0].height);
	if (ycbcrBuffer.cb.size() != buffer[1].width * buffer[1].height)
		ycbcrBuffer.cb.resize(buffer[1].width * buffer[1].height);
	if (ycbcrBuffer.cr.size() != buffer[2].width * buffer[2].height)
		ycbcrBuffer.cr.resize(buffer[2].width * buffer[2].height);

	for (int i = 0; i < buffer[0].height; ++i) {
		std::memcpy(
			ycbcrBuffer.y.data() + i * buffer[0].width,
			buffer[0].data + i * buffer[0].stride,
			buffer[0].width
		);
	}
	for (int i = 0; i < buffer[1].height; ++i) {
		std::memcpy(
			ycbcrBuffer.cb.data() + i * buffer[1].width,
			buffer[1].data + i * buffer[1].stride,
			buffer[1].width
		);
	}
	for (int i = 0; i < buffer[2].height; ++i) {
		std::memcpy(
			ycbcrBuffer.cr.data() + i * buffer[2].width,
			buffer[2].data + i * buffer[2].stride,
			buffer[2].width
		);
	}
}

bool TheoraStream::eos() const {
	return OggStream::eos();
}

}
