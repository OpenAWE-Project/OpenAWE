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

#ifndef OPENAWE_OBJFILE_H
#define OPENAWE_OBJFILE_H

#include "src/common/readstream.h"

#include <memory>

namespace AWE {

class OBJFile {
public:
	struct Shader {
		std::unique_ptr<Common::ReadStream> vertexShader;
		std::unique_ptr<Common::ReadStream> pixelShader;
	};

	struct Program {
		std::string name;
		std::vector<Shader> shaders;
	};

	OBJFile(Common::ReadStream *obj);

	const std::string &getName();
	std::vector<Program> &getPrograms(){return _programs;};

private:
	std::string _name;
	std::vector<Program> _programs;

	std::unique_ptr<Common::ReadStream> _obj;
};

} // End of namspace AWE

#endif //OPENAWE_OBJFILE_H
