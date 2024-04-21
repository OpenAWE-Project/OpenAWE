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

#ifndef OPENAWE_SKY_H
#define OPENAWE_SKY_H

#include "model.h"

namespace Graphics {

class Sky;

typedef std::shared_ptr<Sky> SkyPtr;

/*!
 * \brief Class representing the sky in the scenery
 *
 * This class represents the sky and manages all geometry, textures and uniform
 * values necessary for generating the sky according to the variables.
 */
class Sky {
public:
	Sky();

	/*!
	 * Get the mesh of the sky
	 *
	 * \return The mesh of the sky
	 */
	[[nodiscard]] const MeshPtr &getSkyMesh() const;

	/*!
	 * Get all special uniforms which are not statically added to the mesh and can change
	 *
	 * \return The non-mesh uniforms for the sky
	 */
	[[nodiscard]] const std::vector<Material::Uniform> &getUniforms() const;

private:
	Graphics::MeshPtr _sky;
	std::vector<Material::Uniform> _uniforms;
};

} // End of namespace Graphics

#endif //OPENAWE_SKY_H
