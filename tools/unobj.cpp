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

#include <cstdlib>

#include <filesystem>
#include <format>

#include <fmt/format.h>
#include <CLI/CLI.hpp>

#include "src/common/readfile.h"
#include "src/common/writefile.h"

#include "src/awe/objfile.h"

int main(int argc, char** argv) {
	CLI::App app("Unpack obj shader archives", "unobj");

	std::string objFile;

	app.add_option("objarchive", objFile, "The obj shader archive")
			->check(CLI::ExistingFile)
			->required();

	CLI11_PARSE(app, argc, argv);

	AWE::OBJFile obj(new Common::ReadFile(objFile));

	const auto programName = obj.getName();
	for (const auto &program : obj.getPrograms()) {
		for (const auto &shader : program.shaders) {
			const auto shaderId = std::format(
				"{}_{}_{:0>8x}",
				programName,
				program.name,
				shader.flags
			);

			const auto vertShader = std::format("{}.vert.fxo", shaderId);
			const auto fragShader = std::format("{}.frag.fxo", shaderId);

			Common::WriteFile vertShaderFile(vertShader);
			Common::WriteFile fragShaderFile(fragShader);

			vertShaderFile.writeStream(&*shader.vertexShader);
			fragShaderFile.writeStream(&*shader.pixelShader);
		}
	}

	return EXIT_SUCCESS;
}
