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
#include "src/common/exception.h"
#include "src/common/strutil.h"

#include "src/awe/rmdparchive.h"
#include "src/awe/path.h"

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

	fmt::print("Processing index...\n");

	AWE::RMDPArchive rmdp(
		new Common::ReadFile(binFile),
		new Common::ReadFile(rmdpFile)
	);

	for (size_t i = 0; i < rmdp.getNumResources(); ++i) {
		const std::string path = rmdp.getResourcePath(i);
		const std::string normalizedPath = AWE::getNormalizedPath(path);

		fmt::print("{}/{} {}\n", i + 1, rmdp.getNumResources(), normalizedPath);

		if (!onlyListFiles) {
			const auto resourceStream = std::unique_ptr<Common::ReadStream>(rmdp.getResource(Common::toLower(path)));
			if (!resourceStream.get())
				throw Common::Exception("Resource not found in archive: {}", path);

			std::filesystem::path p(normalizedPath);
			if (!p.parent_path().empty())
				std::filesystem::create_directories(p.parent_path());

			Common::WriteFile writeFile(p.string());
			writeFile.writeStream(resourceStream.get());
			writeFile.close();
		}
	}

	return EXIT_SUCCESS;
}
