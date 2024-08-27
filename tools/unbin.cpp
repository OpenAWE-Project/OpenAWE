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

#include "src/awe/binarchive.h"

int main(int argc, char** argv) {
	CLI::App app("Unpack bin archive", "unbin");

	std::string binFile;
	bool onlyListFiles = false;

	app.add_option("binarchive", binFile, "The bin file archive")
			->check(CLI::ExistingFile)
			->required();

	app.add_flag("-l, --list", onlyListFiles, "List files in an archive without actually extracting them");

	CLI11_PARSE(app, argc, argv);

	std::unique_ptr<Common::ReadStream> binStream = std::make_unique<Common::ReadFile>(binFile);
	AWE::BINArchive bin(*binStream);

	for (size_t i = 0; i < bin.getNumResources(); ++i) {
		const std::string path = bin.getResourcePath(i);

		fmt::print("{}/{} {}\n", i + 1, bin.getNumResources(), path);

		if (!onlyListFiles) {
			const auto resourceStream = std::unique_ptr<Common::ReadStream>(bin.getResource(path));

			Common::WriteFile writeFile(path);
			writeFile.writeStream(resourceStream.get());
			writeFile.close();
		}
	}

	return EXIT_SUCCESS;
}
