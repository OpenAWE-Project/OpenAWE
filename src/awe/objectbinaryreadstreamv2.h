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

#ifndef OPENAWE_OBJECTBINARYREADSTREAMV2_H
#define OPENAWE_OBJECTBINARYREADSTREAMV2_H

#include "src/awe/objectstream.h"

namespace AWE {

class ObjectBinaryReadStreamV2 : public ObjectBinaryReadStream {
public:
	ObjectBinaryReadStreamV2(Common::ReadStream &stream, std::shared_ptr<DPFile> dp = std::shared_ptr<DPFile>());

	Object readObject(ObjectType type, unsigned int version = 0) override;

private:
	uint32_t getContentHash(ObjectType type) const;
};

} // End of namespace AWE

#endif //OPENAWE_OBJECTBINARYREADSTREAMV2_H
