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

#if WITH_LZ4
#	include <lz4.h>
#endif

#include "src/common/lz4.h"
#include "src/common/memreadstream.h"
#include "src/common/exception.h"

namespace Common {

ReadStream *decompressLZ4(const byte *data, size_t compressedSize, size_t decompressedSize) {
#if WITH_LZ4
	byte *decompressedData = new byte[decompressedSize];

	const auto decompressed = LZ4_decompress_safe(
		reinterpret_cast<const char*>(data),
		reinterpret_cast<char*>(decompressedData),
		compressedSize,
		decompressedSize
	);

	if (decompressed != static_cast<long>(decompressedSize)) {
		throw CreateException(
			"LZ4 decompression failed expected {}, got {}",
			decompressedSize,
			decompressed
		);
	}

	return new MemoryReadStream(decompressedData, decompressedSize);
#else
	throw CreateException("LZ4 support was not enabled");
#endif
}

} // End of namespace Common
