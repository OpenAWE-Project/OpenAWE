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

namespace Graphics {

class Buffer;

typedef std::shared_ptr<Buffer> BufferPtr;

class Buffer : Common::Noncopyable {
public:
	virtual void read(byte *data, size_t length) = 0;
	virtual void write(byte *data, size_t length) = 0;
};

} // End of namespace Graphics

#endif //OPENAWE_BUFFER_H
