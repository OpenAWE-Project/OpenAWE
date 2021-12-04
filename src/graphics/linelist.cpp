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

#include "src/graphics/linelist.h"
#include "src/graphics/gfxman.h"

namespace Graphics {

LineList::LineList() {
	_mesh = std::make_shared<Mesh>();

	show();
}

void LineList::clear() {
	_oldLines = _lines;
	_lines.clear();
}

void LineList::flush() {
	if (_lines == _oldLines)
		return;

	std::vector<glm::vec3> lineData;
	for (const auto &line : _lines) {
		lineData.push_back(line.from);
		lineData.push_back(line.color);
		lineData.push_back(line.to);
		lineData.push_back(line.color);
	}

	if (!_buffer) {
		const std::vector<VertexAttribute> attributes = {
				{kPosition, kVec3F},
				{kColor,    kVec3F}
		};

		const std::vector<Material::Attribute> materialAttributes{
		};

		Mesh::PartMesh partMesh;

		partMesh.renderType = Mesh::kLines;
		partMesh.offset = 0;
		partMesh.length = _lines.size() * 2;

		_buffer = GfxMan.createBuffer(
				reinterpret_cast<byte *>(lineData.data()),
				lineData.size() * sizeof(glm::vec3),
				kVertexBuffer,
				true
		);
		partMesh.vertexData = _buffer;
		partMesh.material = Material("color", materialAttributes);
		partMesh.material.setCullMode(Material::kNone);
		partMesh.vertexAttributes = GfxMan.createAttributeObject("color", attributes, partMesh.vertexData);

		_mesh->addPartMesh(partMesh);
	} else {
		_buffer->write(reinterpret_cast<byte *>(lineData.data()), lineData.size() * sizeof(glm::vec3));
	}
}

void LineList::addLine(glm::vec3 from, glm::vec3 to, glm::vec3 color) {
	_lines.emplace_back(
		Line {
			from, to, color
		}
	);
}

bool LineList::Line::operator==(const LineList::Line &rhs) const {
	return std::tie(from, to, color) == std::tie(rhs.from, rhs.to, rhs.color);
}

bool LineList::Line::operator!=(const LineList::Line &rhs) const {
	return !(rhs == *this);
}
}
