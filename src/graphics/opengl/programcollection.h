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

#ifndef OPENAWE_PROGRAMCOLLECTION_H
#define OPENAWE_PROGRAMCOLLECTION_H

#include <vector>

#include "src/graphics/opengl/program.h"

namespace Graphics::OpenGL {

/*!
 * \brief Helper class for organizing shaders
 *
 * This class is responsible for storing progrfrom a specific shader type according to stage and properties and allow
 * easy retrieval of the programs for the rendering process
 */
class ProgramCollection {
public:
	ProgramCollection();

	/*!
	 * Check if there is a program available for a specific stage
	 * \param stage The stage to check for
	 * \return If a program  exists for the specific stage
	 */
	bool hasStage(const std::string &stage);

	/*!
	 * Set the program for a slot at a specific stage
	 * \param stage The stage for the program to set
	 * \param program The program to set for the specific stage
	 */
	void setProgram(const std::string &stage, ProgramPtr program);

	/*!
	 * Get the program associated with a specific stage
	 * \param stage The stage of the program to get
	 * \return A pointer to the program to get
	 */
	ProgramPtr getProgram(const std::string &stage) const;

private:
	std::map<std::string, ProgramPtr> _programs;
};

}; // End of namespace Graphics::OpenGL

#endif //OPENAWE_PROGRAMCOLLECTION_H
