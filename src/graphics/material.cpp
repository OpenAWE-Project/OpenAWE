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

#include "material.h"
#include "gfxman.h"

namespace Graphics {

Material::Material(const std::string &shaderName, std::vector<Attribute> attributes, uint32_t properties) :
	_shaderName(shaderName),
	_attributes(attributes),
	_blendMode(BlendMode::kNone),
	_properties(properties) {
	for (auto &attribute: _attributes) {
		attribute.index = GfxMan.getUniformIndex(_shaderName, attribute.id);
	}
}

const std::vector<Material::Attribute> &Material::getAttributes() const {
	return _attributes;
}

const std::string &Material::getShaderName() const {
	return _shaderName;
}

void Material::setCullMode(Material::CullMode cullMode) {
	_cullMode = cullMode;
}

Material::CullMode Material::getCullMode() const {
	return _cullMode;
}

BlendMode Material::getBlendMode() const {
	return _blendMode;
}

void Material::setBlendMode(BlendMode blendMode) {
	_blendMode = blendMode;
}

uint32_t Material::getProperties() const {
	return _properties;
}

}
