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

#ifndef OPENAWE_LIGHT_H
#define OPENAWE_LIGHT_H

#include <string>
#include <optional>

#include "src/graphics/buffer.h"
#include "src/graphics/attributeobject.h"

namespace Graphics {

/*!
 * \brief Class representing a light in the scene
 *
 * This class has all data necessary for representing a light in the scene. It is build up of a 3d shape representing
 * the area of effect and various parameters like the color.
 *
 * The falloff function for pointlights is defined as \f$2^{\delta\cdot ln(d)}-f\f$ with \f$\delta\f$ being the
 * distance to the pointlights center and \f$d\f$ being the decay of the light
 */
class Light {
public:
	Light();

	/*!
	 * Adds the light to the scene graph and makes it through that visible
	 */
	void show();

	/*!
	 * Return the current color of the light
	 * \return The lights color
	 */
	const glm::vec3 &getColor() const;

	/*!
	 * Get the vector of falloff values of the light
	 * \return The lights falloff vector
	 */
	glm::vec4 getFalloff() const;

	/*!
	 * Get the intensity of the light
	 * \return The intensity of the light
	 */
	float getIntensity() const;

	/*!
	 * Set the color of the light
	 * \param color The color of the light
	 */
	void setColor(const glm::vec3 &color);

	/*!
	 * Set the decay of the light
	 * \param decay The new decay of the light
	 */
	void setDecay(float decay);

	/*!
	 * Set the directional falloff
	 * \param directionalFalloff The new directional falloff for the light
	 */
	void setDirectionalFalloff(float directionalFalloff);

	/*!
	 * Set the intensity of the light
	 * \param intensity The new intensity of the light
	 */
	void setIntensity(float intensity);

	/*!
	 * Get the transform of the light
	 * \return A matrix expressing the transform of the light
	 */
	glm::mat4 getTransform() const;

	/*!
	 * Get the buffer of the shapes indices
	 * \return The buffer of the shapes indices
	 */
	const BufferPtr &getIndices() const;

	/*!
	 * Get the number of indices in the lights shape
	 * \return The number of indices
	 */
	unsigned int getNumIndices() const;

	/*!
	 * Set the transform matrix for the light
	 * \param transform The new transform matrix for the light
	 */
	void setTransform(const glm::mat4 &transform);

	/*!
	 * Get the attribute object for the light
	 * \return The attribute object of the light
	 */
	const AttributeObjectPtr &getAttributeObject() const;

    /*!
     * Get the current label of the light
     * \return The label of the light
     */
	const std::string &getLabel() const;

    /*!
     * Set the label of the light
     * \param label The new label of the light
     */
	void setLabel(const std::string &label);

	/*!
	 * Set a value where to clip the range
	 * @param rangeClip
	 */
	void setRangeClip(float rangeClip);

	/*!
	 * Get the maximum range, this light will affect
	 * \return The maximum range, this light will affect
	 */
	float getRange() const;

	/*!
	 * Check if this light is enabled. If it si not, it won't be rendered
	 * \return If the light is enabled
	 */
	bool isEnabled() const;

	/*!
	 * Set if this light should be enabled.
	 * \param enabled If the light should be enabled or not
	 */
	void setEnabled(bool enabled);

private:
	std::string _label;

    std::optional<float> _rangeClip;

	bool _enabled;
	float _decay;
	float _intensity;
	float _directionalFalloff;
	glm::vec3 _color;
	glm::mat4 _transform;
	BufferPtr _shape;
	BufferPtr _indices;
	unsigned int _numIndices;
	AttributeObjectPtr _attribute;
};

} // End of namespace Graphics

#endif //OPENAWE_LIGHT_H
