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

#include <filesystem>

#include <glm/gtx/transform.hpp>

#include "src/common/exception.h"
#include "src/common/readfile.h"
#include "src/common/toml.h"
#include "src/common/xml.h"
#include "src/common/memwritestream.h"

#include "src/awe/resman.h"

#include "src/graphics/material.h"
#include "src/graphics/meshman.h"
#include "src/graphics/sky.h"
#include "src/graphics/gfxman.h"

namespace Graphics {

Sky::Sky() : _sky(MeshMan.getMesh("objects/hardcoded/sky.binfbx", {"night_sky"})) {
	_uniforms.emplace_back("g_fFar", 1000.0f);

	for (auto &uniform: _uniforms) {
		uniform.index = GfxMan.getUniformIndex("sky", "night_sky", 0, uniform.id);
	}
}

void Sky::setTime(int hour, int minute) {
	if (hour > 23)
		throw CreateException("Invalid hour, expected <=23, got {}", hour);

	if (minute > 59)
		throw CreateException("Invalid minute, expected <=59, got {}", minute);

	_daytime = static_cast<float>(hour) / 24.0f + (static_cast<float>(minute) / 60.0f) * (1.0f / 24.0f);
}

const MeshPtr &Sky::getSkyMesh() const {
	return _sky;
}

const std::vector<Material::Uniform> &Sky::getUniforms() const {
	return _uniforms;
}

} // End of namespace Graphics