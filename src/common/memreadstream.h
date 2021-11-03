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

#ifndef SRC_COMMON_MEMREADSTREAM_H
#define SRC_COMMON_MEMREADSTREAM_H

#include <sstream>

#include "src/common/readstream.h"

namespace Common {

class MemoryReadStream : public ReadStream {
public:
	MemoryReadStream(byte *data, size_t length, bool dispose = true);
	MemoryReadStream(const byte *data, size_t length);
	MemoryReadStream(const char *data, size_t length);
	~MemoryReadStream();

	size_t read(void *data, size_t length) override;

	size_t pos() const override;

	bool eos() const override;

	void seek(ptrdiff_t length, SeekOrigin origin) override;

private:
	bool _dispose;
	const byte *_data;
	size_t _size, _position;
};

} // End of namespace Common

#endif // SRC_COMMON_MEMREADSTREAM_H
