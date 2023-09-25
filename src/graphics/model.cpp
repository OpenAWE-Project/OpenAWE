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

#include "src/graphics/model.h"
#include "src/graphics/gfxman.h"
#include "src/graphics/meshman.h"
#include "src/graphics/skeleton.h"

#include "src/awe/havokfile.h"

namespace Graphics {

Model::Model() : _numInstances(1), _label("<No Label>"), _mesh(new Mesh), _transform(glm::identity<glm::mat4>()) {

}

Model::Model(rid_t rid) : _numInstances(1), _label("<No Label>"), _mesh(MeshMan.getMesh(rid)), _transform(glm::identity<glm::mat4>()) {
	show();
}

Model::Model(const std::string &path) : _numInstances(1), _label("<No Label>"),  _mesh(MeshMan.getMesh(path)), _transform(glm::identity<glm::mat4>()) {
	show();
}

Model::Model(MeshPtr mesh) : _numInstances(1), _label("<No Label>"), _mesh(mesh) {
	show();
}

Model::~Model() {
	hide();
}

void Model::show() {
	GfxMan.addModel(this);
}

void Model::hide() {
	GfxMan.removeModel(this);
}

void Model::setTransform(const glm::mat4 &transform) {
	_transform = transform;
	_invTransform = glm::inverse(transform);
}

glm::mat4 Model::getTransform() const {
	return _transform;
}

glm::mat4 Model::getInverseTransform() const {
	return _invTransform;
}

MeshPtr Model::getMesh() const {
	return _mesh;
}

void Model::setSkeleton(const Skeleton &skeleton) {
	_skeleton = std::make_unique<Skeleton>(skeleton);
	_skeleton->setInverseTransform(_mesh->getInverseRestTransforms());
}

const Skeleton &Model::getSkeleton() const {
	return *_skeleton;
}

bool Model::hasSkeleton() const {
	return !!_skeleton;
}

void Model::setLabel(const std::string &label) {
	_label = label;
}

const std::string &Model::getLabel() const {
	return _label;
}

unsigned int Model::getNumInstances() const {
	return _numInstances;
}

void Model::setNumInstances(unsigned int numInstances) {
	_numInstances = numInstances;
}

std::vector<Material::Uniform> Model::getUniforms(
		const std::string &stage,
		const std::string &shader,
		uint32_t properties
) const {
	if (_uniforms.find({stage, shader, properties}) == _uniforms.end())
		return {};
	return _uniforms.at({stage, shader, properties});
}

void Model::addModelUniform(Material::Uniform uniform) {
	for (const auto &subMesh: _mesh->getMeshs()) {
		for (const auto &stage: subMesh.material.getStages()) {
			const auto index = GfxMan.getUniformIndex(
				subMesh.material.getShaderName(),
				stage,
				subMesh.material.getProperties(),
				uniform.id
			);

			if (index >= 0)
				_uniforms[{
					stage,
					subMesh.material.getShaderName(),
					subMesh.material.getProperties()
				}].emplace_back(uniform).index = index;
		}
	}
}

bool Model::hasBoundSphere() const {
	return _boundSphere || _mesh->hasBoundingSphere();
}

const Common::BoundSphere & Model::getBoundSphere() const {
	if (_boundSphere)
		return *_boundSphere;
	else
		return _mesh->getBoundingSphere();
}

void Model::setBoundSphere(const Common::BoundSphere &boundSphere) {
	_boundSphere = boundSphere;
}

}
