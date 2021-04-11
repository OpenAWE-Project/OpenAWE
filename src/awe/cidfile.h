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

#ifndef AWE_CIDFILE_H
#define AWE_CIDFILE_H

#include <variant>
#include <optional>

#include <glm/glm.hpp>
#include <glm/vec3.hpp>
#include <glm/detail/type_quat.hpp>

#include "src/common/readstream.h"

#include "src/awe/dpfile.h"
#include "src/awe/types.h"
#include "src/awe/objectstream.h"

namespace AWE {

/*!
 * \brief Load files prefixed with cid_
 *
 * The files prefixed with cid_ contain the major data
 * of Alan Wake and Alan Wakes American Nightmare and are
 * defined by their header. The rest of the data is a
 * standard object stream as used by save games or packmeta
 * files. There are exceptions from this format like
 * terraindata or roadmap file. Please refer to their
 * specialized classes for more info.
 */
class CIDFile {
public:
	CIDFile(Common::ReadStream &cid, ObjectType type, std::shared_ptr<DPFile> dp = nullptr);

	[[nodiscard]] const std::vector<Object> &getContainers() const;

private:
	enum FileFormat {
		kSimple,
		kStructured,
		kStructuredV2
	};

	void testFormat(Common::ReadStream &cid);

	FileFormat _format;
	std::unique_ptr<ObjectReadStream> _objectStream;

	std::shared_ptr<DPFile> _dp;
	std::vector<Object> _containers;
};

} // End of namespace AWE

#endif //AWE_CIDFILE_H
