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

#include "src/common/exception.h"

#include "src/graphics/opengl/programcollection.h"

namespace Graphics::OpenGL {

ProgramCollection::ProgramCollection() {
}

bool ProgramCollection::hasStage(const std::string &stage) {
	return _programs.find(stage) != _programs.end();
}

void ProgramCollection::setProgram(const std::string &stage, ProgramPtr program) {
	_programs[stage] = program;
}

ProgramPtr ProgramCollection::getProgram(const std::string &stage) const {
	const auto programIter = _programs.find(stage);
	if (programIter == _programs.cend())
		throw CreateException("Couldn't find program for stage {}", stage);

	return programIter->second;
}

} // End of namespace Graphics::OpenGL
