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

#include <string.h>

#include <vector>
#include <memory>
#include <regex>

#include <mojoshader.h>
#include <fmt/format.h>
#include <spdlog/spdlog.h>

#include "src/common/types.h"
#include "src/common/memwritestream.h"

#include "src/graphics/shaderconverter.h"

namespace Graphics {

ShaderConverter::ShaderConverter(Common::ReadStream &shader) {
	Common::DynamicMemoryWriteStream dxbc(true);
	std::unique_ptr<Common::ReadStream> stream(shader.readStream());
	dxbc.writeStream(stream.get());
	_dxbc.resize(dxbc.getLength());
	memcpy(_dxbc.data(), dxbc.getData(), dxbc.getLength());
}

const std::vector<ShaderConverter::Symbol> &ShaderConverter::getSymbols() {
	return _symbols;
}

const std::map<std::string, std::string> &ShaderConverter::getSamplerMappings() {
	return _samplers;
}

const std::map<AttributeType, std::string> &ShaderConverter::getAttributeMappings() {
	return _attributes;
}

std::string ShaderConverter::convertToGLSL() {
	const MOJOSHADER_parseData *parseData;

	parseData = MOJOSHADER_parse(
			MOJOSHADER_PROFILE_GLSL120,
			nullptr,
			_dxbc.data(),
			_dxbc.size(),
			nullptr,
			0,
			nullptr,
			0,
			nullptr,
	        nullptr,
	        nullptr
	);

	std::string shader;
	if (parseData->error_count > 0) {
		std::string errorMessage;
		for (int i = 0; i < parseData->error_count; ++i) {
			errorMessage += fmt::format("Error ({},{}): {}",
							   parseData->errors[i].filename ? parseData->errors[i].filename : "???",
							   parseData->errors[i].error_position,
							   parseData->errors[i].error
			);
		}
		throw std::runtime_error(errorMessage.c_str());
	} else {
		shader = std::string (parseData->output, parseData->output_len);
		ShaderType shaderType;
		switch (parseData->shader_type) {
			default:
			case MOJOSHADER_TYPE_VERTEX:
				shaderType = kVertex;
				break;
			case MOJOSHADER_TYPE_PIXEL:
				shaderType = kPixel;
				break;
		}

		// Replace attribute names with mor appropriate names
		unsigned int uvIndex = 0;
		unsigned int colorIndex = 0;
		if (parseData->shader_type == MOJOSHADER_TYPE_VERTEX) {
			for (int i = 0; i < parseData->attribute_count; ++i) {
				const auto &attribute = parseData->attributes[i];

				if (attribute.usage == MOJOSHADER_USAGE_UNKNOWN)
					continue;

				switch (attribute.usage) {
					case MOJOSHADER_USAGE_POSITION:
						_attributes[kPosition] = attribute.name;
						break;
					case MOJOSHADER_USAGE_NORMAL:
						_attributes[kNormal] = attribute.name;
						break;
					case MOJOSHADER_USAGE_COLOR:
						_attributes[getColor(colorIndex)] = attribute.name;
						colorIndex++;
						break;
					case MOJOSHADER_USAGE_BLENDINDICES:
						_attributes[kBoneIndex] = attribute.name;
						break;
					case MOJOSHADER_USAGE_BLENDWEIGHT:
						_attributes[kBoneWeight] = attribute.name;
						break;
					case MOJOSHADER_USAGE_TANGENT:
						_attributes[kTangent] = attribute.name;
						break;
					case MOJOSHADER_USAGE_TEXCOORD:
						_attributes[getTexCoord(uvIndex)] = attribute.name;
						uvIndex++;
						break;
					default:
						throw std::runtime_error("Invalid usage");
				}
			}
		}
		
		// Add symbol mappings
		for (int i = 0; i < parseData->symbol_count; ++i) {
			const auto symbol = parseData->symbols[i];

			Symbol s{};
			s.name = symbol.name;
			s.shaderType = shaderType;
			s.index = symbol.register_index;
			s.length = symbol.register_count;

			if (symbol.register_set == MOJOSHADER_SYMREGSET_SAMPLER) {
				for (int j = 0; j < parseData->sampler_count; ++j) {
					const auto sampler = parseData->samplers[j];
					if (symbol.register_count <= 1) {
						if (sampler.index == symbol.register_index) {
							_samplers[symbol.name] = sampler.name;
						}
					} else {
						if (sampler.index >= symbol.register_index && sampler.index < symbol.register_index + symbol.register_count) {
							_samplers[fmt::format("{}[{}]", symbol.name, sampler.index - symbol.register_index)] = sampler.name;
						}
					}
				}
				continue;
			}

			_symbols.emplace_back(s);
		}

		std::string shaderTypeName = "unknown";
		switch (parseData->shader_type) {
			case MOJOSHADER_TYPE_VERTEX:
				shaderTypeName = "vertex";
				break;
			case MOJOSHADER_TYPE_PIXEL:
				shaderTypeName = "pixel";
				break;
		}

		spdlog::debug("Converted {} shader to glsl:\n{}", shaderTypeName, shader);
	}

	MOJOSHADER_freeParseData(parseData);

	return shader;
}

std::vector<byte> ShaderConverter::convertToSpirv() {
	const MOJOSHADER_parseData *parseData;

	parseData = MOJOSHADER_parse(
			MOJOSHADER_PROFILE_SPIRV,
			nullptr,
			_dxbc.data(),
			_dxbc.size(),
			nullptr,
			0,
			nullptr,
			0,
			nullptr,
			nullptr,
			nullptr
	);

	MOJOSHADER_freeParseData(parseData);

	return std::vector<byte>();
}

}
