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

#ifndef OPENAWE_LINELIST_H
#define OPENAWE_LINELIST_H

#include "src/graphics/model.h"

namespace Graphics {

class LineList : public Model {
public:
	LineList();

	void clear();
	void flush();
	void addLine(glm::vec3 from, glm::vec3 to, glm::vec3 color);

private:
	struct Line {
		glm::vec3 from, to;
		glm::vec3 color;

		bool operator==(const Line &rhs) const;
		bool operator!=(const Line &rhs) const;
	};
	std::vector<Line> _lines, _oldLines;
};

} // End of namespace

#endif //OPENAWE_LINELIST_H
