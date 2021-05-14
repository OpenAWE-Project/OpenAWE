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

#ifndef OPENAWE_SHADERCOMPILER_H
#define OPENAWE_SHADERCOMPILER_H

#include <memory>
#include <map>

#include "src/common/readstream.h"

#include "src/graphics/types.h"

namespace Graphics {

/*!
 * \brief This class is responsible for converting dxbc bytecode
 *
 * This class uses the mojoshader library to convert dxbc proprietary byte code
 * to other languages to utilize them in other rendering systems, like glsl, msl
 * or spirv.
 */
class ShaderConverter {
public:
	enum ShaderType {
		kPixel,
		kVertex
	};

	struct Symbol {
		ShaderType shaderType;
		std::string name;
		unsigned int index;
		unsigned int length;
	};

	ShaderConverter(Common::ReadStream &shader);

	const std::vector<Symbol> &getSymbols();
	const std::map<std::string, std::string> &getSamplerMappings();
	const std::map<AttributeType, std::string> &getAttributeMappings();

	std::string convertToGLSL();
	std::vector<byte> convertToSpirv();

private:
	std::vector<byte> _dxbc;

	std::vector<Symbol> _symbols;
	std::map<std::string, std::string> _samplers;
	std::map<AttributeType, std::string> _attributes;
};

}

#endif //OPENAWE_SHADERCOMPILER_H
