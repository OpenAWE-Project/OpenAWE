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

#ifndef OPENAWE_UNIFORMBUFFERWRITER_H
#define OPENAWE_UNIFORMBUFFERWRITER_H

#include <map>

#include "src/common/types.h"

#include "src/graphics/types.h"

namespace Graphics {
/**
 * \brief A helper class for creating a data block in accordance to the std140 layout
 *
 * This class allows to write a block of data in accordance to the std140 layout for uniform buffers and uses a uniform
 * block description for the necessary information.
 */
class UniformBufferWriter {
public:
	UniformBufferWriter(const UniformBlockDescription &descr);

	/**
	 * Set a uniform float given by its id in this buffer
	 *
	 * \param id The name of the uniform to set
	 * \param value The float value to set
	 */
	void setFloat(const std::string &id, float value);

	/**
	 * Set a uniform vec2 given by its id in this buffer
	 *
	 * \param id The name of the uniform to set
	 * \param value The vec2 value to set
	 */
	void setVec2(const std::string &id, glm::vec2 value);

	/**
	 * Set a uniform vec3 given by its id in this buffer
	 *
	 * \param id The name of the uniform to set
	 * \param value The vec3 value to set
	 */
	void setVec3(const std::string &id, glm::vec3 value);

	/**
	 * Set a uniform vec4 given by its id in this buffer
	 *
	 * \param id The name of the uniform to set
	 * \param value The vec4 value to set
	 */
	void setVec4(const std::string &id, glm::vec4 value);

	/**
	 * Get the current data buffer for further usage
	 *
	 * \return The current data buffer
	 */
	std::span<std::byte> getData();

private:
	size_t itemOffset(const std::string &id) const;

	const UniformBlockDescription _descr;
	const std::vector<std::string> _index;
	const std::vector<size_t> _offsets;

	Common::ByteBuffer _data;
};
} // End of namespace Graphics

#endif //OPENAWE_STD430WRITER_H
