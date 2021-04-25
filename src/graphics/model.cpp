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

#include "src/awe/havokfile.h"

namespace Graphics {

Model::Model() : _mesh(new Mesh), _position(0.0f), _rotation(1.0f), _scale(1.0f) {
}

Model::Model(rid_t rid) : _position(0.0f, 0.0f, 0.0f), _rotation(0.0f), _scale(1.0f),
_mesh(MeshMan.getMesh(rid)) {
	show();
}

Model::Model(const std::string &path)  : _position(0.0f, 0.0f, 0.0f),
_rotation(1.0f), _scale(1.0f), _mesh(MeshMan.getMesh(path)) {
	show();
}

Model::Model(MeshPtr mesh) :_position(0.0f, 0.0f, 0.0f),
                            _rotation(1.0f), _scale(1.0f), _mesh(mesh) {
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

glm::mat3 &Model::getRotation(){
	return _rotation;
}

glm::vec3 &Model::getPosition() {
	return _position;
}

glm::vec3 &Model::getScale() {
	return _scale;
}

MeshPtr Model::getMesh() const {
	return _mesh;
}

}
