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

#include "src/common/exception.h"

#include "src/awe/objectbinaryreadstreamv1.h"

namespace AWE {

ObjectBinaryReadStreamV1::ObjectBinaryReadStreamV1(Common::ReadStream &stream, std::shared_ptr<DPFile> dp) : ObjectBinaryReadStream(stream, std::move(dp)) {
}

Object ObjectBinaryReadStreamV1::readObject(ObjectType type, unsigned int version) {
	Object object;
	ObjectStream::object(object, type, version);

	return object;
}

} // End of namespace AWE
