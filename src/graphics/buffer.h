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

#ifndef OPENAWE_BUFFER_H
#define OPENAWE_BUFFER_H

#include <memory>

#include "src/common/types.h"

#include "src/graphics/std140layout.h"

namespace Graphics {

class Buffer;

typedef std::shared_ptr<Buffer> BufferPtr;

/*!
 * Layout type to use for uniform buffer layout. Different rendering systems require different layouts
 */
enum class LayoutType {
	kStd140
};

class Buffer : Common::Noncopyable {
public:
	[[deprecated("Not thread safe, will probably reworked or removed")]]
	virtual void read(byte *data, size_t length) = 0;

	/*!
	 * Instruct the buffer to load a buffer of data. This call is async, it is not guaranteed that the loading is done
	 * when it returns
	 *
	 * @param data The data to load
	 */
	virtual void write(Common::ByteBuffer &&data) = 0;

	/*!
	 * Instruct the buffer to load a buffer of data. This call ensures, that until the next graphics object update, the
	 * data will be written into the buffer. The span needs to stay alive until the write has been executed
	 *
	 * \param data The data to be loaded. Must stay alive until the data is successfully loaded
	 */
	virtual void write(std::span<std::byte> data) = 0;

	/*!
	 * Instruct the buffer to load the given data as a uniform block by the appropriate layout
	 *
	 * \tparam T The type of the uniform to write
	 * \param data The value of the uniform block to write
	 */
	template<typename T>
	void write(const T& data) {
		switch (type) {
			case LayoutType::kStd140:
				write(std140::pack(data));
				break;
		}
	}

private:
	/// The type of the layout to pack uniform buffer
	const LayoutType type;

protected:
	explicit Buffer(LayoutType type) : type(type) {}
};

} // End of namespace Graphics

#endif //OPENAWE_BUFFER_H
