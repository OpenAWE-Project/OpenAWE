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

#include <stdexcept>

#include <vector>

#include "src/common/strutil.h"

#include "src/awe/objfile.h"

AWE::OBJFile::OBJFile(Common::ReadStream *obj) : _obj(obj) {
	std::string magic = obj->readFixedSizeString(4);
	if (magic != "RFX ")
		throw std::runtime_error("Invalid obj file");

	uint32_t version = obj->readUint32LE();
	if (version != 10)
		throw std::runtime_error("Invalid version, only 10 is supported");

	uint32_t numStrings = obj->readUint32LE();
	std::vector<std::string> strings(numStrings);
	for (auto &string : strings) {
		uint32_t lengthString = obj->readUint32LE();
		string = obj->readFixedSizeString(lengthString);
		obj->skip(4); // CRC32 Hash for these files
	}

	_name = std::regex_replace(strings[0], std::regex("\\.rfx"), "");

	uint32_t numPrograms = obj->readUint32LE();
	_programs.resize(numPrograms);
	for (auto &program : _programs) {
		uint32_t programNameLength = obj->readUint32LE();
		program.name = obj->readFixedSizeString(programNameLength);

		uint64_t numShaders = obj->readUint32LE();
		program.shaders.resize(numShaders);
		for (auto &shader : program.shaders) {
			shader.flags = obj->readUint32LE();
			uint32_t vertexShaderFileSize = obj->readUint32LE();
			shader.vertexShader.reset(obj->readStream(vertexShaderFileSize));
			uint32_t pixelShaderFileSize = obj->readUint32LE();
			shader.pixelShader.reset(obj->readStream(pixelShaderFileSize));
		}
	}
}

const std::string &AWE::OBJFile::getName() {
	return _name;
}
