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

#include <glm/gtx/transform.hpp>

#include "src/common/shape.h"

#include "src/graphics/light.h"
#include "src/graphics/gfxman.h"

enum PointLightPropertyFlags {
	kVarianceShadowMap = 0x00000002,
	kDirection         = 0x00000004,
	kGlossiness        = 0x00000008
};

namespace Graphics {

Light::Light() : _label("<No Label>"), _enabled(true) {
	auto shape = Common::generateIcoSphere(1.075, 1);
	Common::reverseTriangles(shape);

	Common::ByteBuffer vertexData(shape.positions.size() * sizeof(glm::vec3));
	Common::ByteBuffer indexData(shape.indices.size() * sizeof(uint16_t));

	std::memcpy(vertexData.data(), shape.positions.data(), vertexData.size());
	std::memcpy(indexData.data(), shape.indices.data(), indexData.size());

	_shape = GfxMan.createBuffer(std::move(vertexData),
								 kVertexBuffer, false);
	_indices = GfxMan.createBuffer(std::move(indexData),
								   kIndexBuffer, false);

	_numIndices = shape.indices.size();

	const std::vector<VertexAttribute> attributes = {
		{kPosition, kVec3F}
	};

	_attribute = GfxMan.createAttributeObject(
		"deferredlight",
		"pointlight",
		0,
		attributes,
		_shape
	);
}

void Light::show() {
	GfxMan.addLight(this);
}

void Light::setTransform(const glm::mat4 &transform) {
	_transform = transform;
}

glm::mat4 Light::getTransform() const {
	glm::mat4 transform = _transform;

	if (_rangeClip)
		transform *= glm::scale(glm::vec3(*_rangeClip));
	else
		// TODO: Find out what the original engine does if no range clip is given
		transform *= glm::scale(glm::vec3(16.0));

	return transform;
}

const BufferPtr &Light::getIndices() const {
	return _indices;
}

const AttributeObjectPtr &Light::getAttributeObject() const {
	return _attribute;
}

unsigned int Light::getNumIndices() const {
	return _numIndices;
}

const glm::vec3 &Light::getColor() const {
	return _color;
}

glm::vec4 Light::getFalloff() const {
	constexpr auto falloffFactor = glm::vec3(
		0.3086,
		0.6094,
		0.082
	);

	return {
		_decay,
		0.05 / glm::compAdd(_intensity * falloffFactor * _color),
		_directionalFalloff * 0.5,
		1.0 - _directionalFalloff * 0.5
	};
}

void Light::setColor(const glm::vec3 &color) {
	_color = color;
}

void Light::setDecay(float decay) {
	_decay = decay;
}

void Light::setDirectionalFalloff(float directionalFalloff) {
	_directionalFalloff = directionalFalloff;
}

void Light::setIntensity(float intensity) {
	_intensity = intensity;
}

float Light::getIntensity() const {
	return _intensity;
}

const std::string &Light::getLabel() const {
	return _label;
}

void Light::setLabel(const std::string &label) {
	if (label.empty())
		_label = "<Light>";
	else
		_label = label;
}

void Light::setRangeClip(float rangeClip) {
	_rangeClip = rangeClip;
}

float Light::getRange() const {
	if (_rangeClip)
		return *_rangeClip;
	else
		return 16.0f;
}

bool Light::isEnabled() const {
	return _enabled;
}

void Light::setEnabled(bool enabled) {
	_enabled = enabled;
}

} // End of namespace Graphics