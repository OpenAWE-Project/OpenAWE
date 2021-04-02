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

#include "font.h"

namespace Graphics {

Common::UUID Graphics::Font::getTexture() const {
	return _texture;
}

Common::UUID Graphics::Font::getVertices() const {
	return _vertices;
}

Common::UUID Graphics::Font::getIndices() const {
	return _indices;
}

Common::UUID Font::getVAO() const {
	return _vao;
}

Font::Font() :
	_texture(Common::UUID::generateNil()),
	_vertices(Common::UUID::generateNil()),
	_indices(Common::UUID::generateNil()),
	_vao(Common::UUID::generateNil()) {
}

}
