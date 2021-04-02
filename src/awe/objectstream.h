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

#ifndef OPENAWE_OBJECTSTREAM_H
#define OPENAWE_OBJECTSTREAM_H

#include <memory>
#include <variant>
#include <any>

#include "src/common/readstream.h"
#include "src/common/types.h"

#include "src/awe/types.h"

namespace AWE {

typedef std::any Object;

class ObjectReadStream : Common::ReadStream {
public:
	ObjectReadStream(ReadStream &objectStream, unsigned int type, unsigned int version);

	virtual Object readObject() = 0;
	virtual void skipObject() = 0;

private:
	const unsigned int _type, _version;
};

class ObjectBinaryReadStream : public ObjectReadStream {
protected:

};

}

#endif //OPENAWE_OBJECTSTREAM_H
