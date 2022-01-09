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

#include <memory>

#include <zlib.h>

#include "src/common/zlib.h"
#include "src/common/memreadstream.h"

namespace Common {

ReadStream *decompressZLIB(const byte *data, size_t compressedSize, size_t decompressedSize) {
	byte *uncompressedData = new byte[decompressedSize];

	z_stream stream;

	stream.zalloc = Z_NULL;
	stream.zfree = Z_NULL;
	stream.opaque = Z_NULL;

	stream.avail_in = compressedSize;
	stream.next_in = const_cast<byte *>(data);
	stream.avail_out = decompressedSize;
	stream.next_out = uncompressedData;

	int result = inflateInit(&stream);
	if (result != Z_OK)
		throw std::runtime_error("Error initializing z_stream");

	result = inflate(&stream, Z_FINISH);
	if (result != Z_STREAM_END)
		throw std::runtime_error("Error inflating");

	inflateEnd(&stream);

	return new MemoryReadStream(uncompressedData, decompressedSize);
}

ReadStream *compressZLIB(byte *data, size_t decompressedSize) {
	z_stream stream;

	stream.zalloc = Z_NULL;
	stream.zfree = Z_NULL;
	stream.opaque = Z_NULL;

	int zResult = deflateInit2(
			&stream,
			Z_BEST_COMPRESSION,
			Z_DEFLATED,
			15,
			9,
			Z_DEFAULT_STRATEGY
	);

	// TODO

	deflateEnd(&stream);

	return nullptr;
}

} // End of namespace Common
