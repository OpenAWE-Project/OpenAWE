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

#include <fmt/format.h>
#include <CLI/CLI.hpp>

#include "src/common/readfile.h"
#include "src/common/writefile.h"

#include "src/graphics/images/tex.h"
#include "src/graphics/images/dumpdds.h"

int main(int argc, char** argv) {
	CLI::App app("Convert a tex image to a dds image", "tex2dds");

	std::string texFile, ddsFile;

	app.add_option("teximage", texFile, "The tex image to convert")
		->check(CLI::ExistingFile)
		->required();

	app.add_option("-o,--output", ddsFile, "The file to output");

	CLI11_PARSE(app, argc, argv);

	if (ddsFile.empty())
		ddsFile = std::filesystem::path(texFile).replace_extension( "dds");

	std::unique_ptr<Common::ReadStream> texStream = std::make_unique<Common::ReadFile>(texFile);
	Graphics::TEX tex(*texStream);

	Common::WriteFile dds(ddsFile);
	Graphics::dumpDDS(dds, tex);
	dds.close();

	return EXIT_SUCCESS;
}
