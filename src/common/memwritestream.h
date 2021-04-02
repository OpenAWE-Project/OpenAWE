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

#ifndef SRC_COMMON_MEMWRITESTREAM_H
#define SRC_COMMON_MEMWRITESTREAM_H

#include "src/common/writestream.h"
#include "src/common/types.h"

namespace Common {

class DynamicMemoryWriteStream : public WriteStream {
public:
	DynamicMemoryWriteStream(bool release);
	~DynamicMemoryWriteStream();

	void write(const void *data, size_t length) override;

	void seek(ptrdiff_t length, SeekOrigin origin) override;

	size_t pos() override;

	size_t getLength();
	byte * getData();

private:
	void extendCapacity();

	bool _release;

	size_t _capacity;
	size_t _size;

	byte *_data;
};

} // End of namespace Common

#endif // SRC_COMMON_MEMWRITESTREAM_H
