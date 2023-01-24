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

#include "src/awe/rmdparchive.h"

int main(int argc, char** argv) {
	CLI::App app("Unpack bin/rmdp archive structure", "unrmdp");

	std::string binFile, rmdpFile;
	bool onlyListFiles = false;

	app.add_option("binfile", binFile, "The bin file containing the archives metadata")
			->check(CLI::ExistingFile)
			->required();

	app.add_option("rmdpfile", rmdpFile, "The rmdp file containing the archives raw data")
			->check(CLI::ExistingFile)
			->required();
	
	app.add_flag("-l, --list", onlyListFiles, "List files in an archive without actually extracting them");

	CLI11_PARSE(app, argc, argv);

	const bool extractFiles = !onlyListFiles;

	AWE::RMDPArchive rmdp(
		new Common::ReadFile(binFile),
		new Common::ReadFile(rmdpFile)
	);

	for (size_t i = 0; i < rmdp.getNumResources(); ++i) {
		const std::string path = rmdp.getResourcePath(i);

		fmt::print("{}/{} {}\n", i + 1, rmdp.getNumResources(), path);

		if (extractFiles) {
			const auto resourceStream = std::unique_ptr<Common::ReadStream>(rmdp.getResource(path));

			std::filesystem::path p(path);
			if (!p.parent_path().empty())
				std::filesystem::create_directories(p.parent_path());

			Common::WriteFile writeFile(p.string());
			writeFile.writeStream(resourceStream.get());
			writeFile.close();
		}
	}

	return EXIT_SUCCESS;
}
