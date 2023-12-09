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

#include "src/common/readstream.h"

namespace Common {

/*!
 * Decompress a certain memory area compressed as lz4 data
 * @param data A pointer to the data to decompress
 * @param compressedSize The size size of the compressed data
 * @param decompressedSize The decompressed sie of the data
 * @return A pointer to a readstream containing the decompressed data.
 */
ReadStream *decompressLZ4(const byte *data, size_t compressedSize, size_t decompressedSize);

} // End of namespace Common
