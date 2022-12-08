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
#include "src/common/xml.h"

#include "src/awe/packmetafile.h"

int main(int argc, char** argv) {
	CLI::App app("Convert packmeta file to xml file", "rdb2xml");

	std::string resourcedbFile;

	app.add_option("binarchive", resourcedbFile, "The packmeta file to convert")
		->check(CLI::ExistingFile)
		->required();

	CLI11_PARSE(app, argc, argv);

	std::unique_ptr<Common::ReadStream> resourcedbStream = std::make_unique<Common::ReadFile>(resourcedbFile);
	AWE::PACKMETAFile packmeta(*resourcedbStream);

	Common::XML xml;
	Common::XML::Node &rootNode = xml.getRootNode();
	rootNode.name = "packmeta";

	for (const auto &rid: packmeta.getRIDs()) {
		Common::XML::Node &resourceNode = rootNode.addNewNode("resource");
		resourceNode.properties["rid"] = fmt::format("0x{:0>8x}", rid);
		resourceNode.properties["path"] = packmeta.getNameByRid(rid);
	}

	Common::WriteFile xmlFile(resourcedbFile + ".xml");
	xml.write(xmlFile, true);
	xmlFile.close();

	return EXIT_SUCCESS;
}
