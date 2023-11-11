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

#ifndef AWE_MODEL_H
#define AWE_MODEL_H

#include <vector>
#include <optional>

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

#include "src/common/readstream.h"

#include "src/graphics/material.h"
#include "src/graphics/mesh.h"
#include "src/graphics/animation.h"

#include "src/awe/types.h"

namespace Graphics {

class Model;
class Skeleton;

typedef std::shared_ptr<Model> ModelPtr;

class Model {
public:
	Model(rid_t rid);
	Model(const std::string &path);
	Model(MeshPtr mesh);
	virtual ~Model();

	/*!
	 * Set a label for the model, which will help to identify it
	 * \param label
	 */
	void setLabel(const std::string &label);

    /*!
     * Get a label of the model
     * \return The label of the model
     */
	const std::string &getLabel() const;

	void show();
	void hide();

	void setTransform(const glm::mat4 &transform);
	glm::mat4 getTransform() const;
	glm::mat4 getInverseTransform() const;

	/*!
	 * Set the visibility state of the model
	 * \param visible If the model should be visible
	 */
	void setVisible(bool visible);

	/*!
	 * Check if the model is currently visible
	 * @return If the model is visible
	 */
	bool isVisible() const;

	/*!
	 * Get the number of instances, this model should be rendered. Defaults to 1
	 * \return The number of instances, this model should be rendered
	 */
	unsigned int getNumInstances() const;

	/*!
	 * Set the number of instances, this model should be rendered
	 * \param numInstances The number of instances, this model should be rendered
	 */
	void setNumInstances(unsigned int numInstances);

	/*!
	 * Get the uniforms associated with a certain tripel of stage shader and properties
	 * \param stage The stage to search uniforms for
	 * \param shader The shader to search uniforms for
	 * \param properties The properties of the shader to search fro
	 * \return The list of uniforms available for this specific render state
	 */
	std::vector<Material::Uniform> getUniforms(
		const std::string &stage,
		const std::string &shader,
		uint32_t properties
	) const;

	/*!
	 * Add a new model uniform which is shared across the complete model
	 * \param uniform The uniform to add as model uniform
	 */
	void addModelUniform(Material::Uniform uniform);

	MeshPtr getMesh() const;
	void setSkeleton(const Skeleton &skeleton);
	const Skeleton &getSkeleton() const;
	bool hasSkeleton() const;

	/*!
	 * Return if the model contains a bounding sphere
	 * \return if the model contains a bounding sphere
	 */
	bool hasBoundSphere() const;

	/*!
	 * Return the bounding sphere associated
	 * \return The global bounding sphere
	 */
	const Common::BoundSphere &getBoundSphere() const;

	/*!
	 * Set the bounding sphere of the model
	 * @param boundSphere
	 */
	void setBoundSphere(const Common::BoundSphere &boundSphere);

protected:
	Model();

	bool _visible{true};
	unsigned int _numInstances;
	std::string _label;

	std::optional<Common::BoundSphere> _boundSphere;

	MeshPtr _mesh;
	std::map<
		std::tuple<std::string, std::string, uint32_t>,
		std::vector<Material::Uniform>
	> _uniforms;
	std::unique_ptr<Skeleton> _skeleton;

	glm::mat4 _transform;
	glm::mat4 _invTransform;
};

}

#endif //AWE_MODEL_H
