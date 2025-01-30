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

#include "src/codecs/dumpwav.h"

#include "src/codecs/fsbfile.h"

int main(int argc, char** argv) {
	CLI::App app("Convert FMOD fsb file to wav file", "fsb2wav");

	std::string binFile;

	app.add_option("fsbarchive", binFile, "The fsb file")
		->check(CLI::ExistingFile)
		->required();

	CLI11_PARSE(app, argc, argv);

	std::unique_ptr<Common::ReadStream> binStream = std::make_unique<Common::ReadFile>(binFile);
	Codecs::FSBFile fsb(new Common::ReadFile(binFile));

	for (size_t i = 0; i < fsb.getNumEntries(); ++i) {
		const std::string path = fsb.getFileName(i);

		fmt::print("{}/{} {}\n", i + 1, fsb.getNumEntries(), path);

		std::unique_ptr<Codecs::SeekableAudioStream> resourceStream(fsb.getStream(i));

		Common::WriteFile wav(path);
		Codecs::dumpWAV(*resourceStream, wav);
	}

	return EXIT_SUCCESS;
}
