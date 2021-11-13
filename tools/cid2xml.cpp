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
#include <cxxopts.hpp>

#include "src/common/writefile.h"
#include "src/common/readfile.h"

#include "src/awe/determinetype.h"
#include "src/awe/cidfile.h"
#include "src/awe/dpfile.h"
#include "src/awe/objectxmlwritestream.h"

int main(int argc, char **argv) {
	cxxopts::Options options(argv[0], "cid2xml - Convert cid files to readable xml");

	options.positional_help("cidfile").allow_unrecognised_options();

	options.add_options()
		("h,help", "Print this help")
		("i,cidfile", "File in cid format to convert", cxxopts::value<std::string>())
		("d,dpfile", "File in dp format to use", cxxopts::value<std::string>()->default_value(""));

	auto result = options.parse(argc, argv);
	if (result.count("help")) {
		std::cout << options.help() << std::endl;
		return EXIT_SUCCESS;
	}

	if (!result.count("cidfile")) {
		std::cerr << "No cid file given" << std::endl;
		return EXIT_FAILURE;
	}

	const auto &dpFile = result["dpfile"].as<std::string>();
	const auto &cidFile = result["cidfile"].as<std::string>();

	if (!std::filesystem::is_regular_file(cidFile)) {
		fmt::print("File {} not found", cidFile);
		return EXIT_FAILURE;
	}

	if (!dpFile.empty() && !std::filesystem::is_regular_file(dpFile)) {
		fmt::print("File {} not found", dpFile);
		return EXIT_FAILURE;
	}

	Common::ReadFile cidFileStream(cidFile);
	Common::XML xml;
	auto &rootNode = xml.getRootNode();
	rootNode.name = "cid";

	const std::string stem = std::filesystem::path(cidFile).stem().string();
	ObjectType type = AWE::determineObjectTypeByFilename(stem);
	std::shared_ptr<DPFile> dp;
	if (!dpFile.empty())
		dp = std::make_shared<DPFile>(new Common::ReadFile(dpFile));

	AWE::ObjectXMLWriteStream xmlWriteStream(xml.getRootNode());
	AWE::CIDFile cid(cidFileStream, type, dp);

	rootNode.properties["version"] = std::to_string(cid.getVersion());

	const auto &containers = cid.getContainers();
	for (const auto &item: containers) {
		xmlWriteStream.writeObject(item, type, cid.getVersion());
	}

	Common::WriteFile cidXmlStream(stem + ".xml");
	xml.write(cidXmlStream, false);

	return EXIT_SUCCESS;
}