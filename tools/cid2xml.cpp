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

#include <CLI/CLI.hpp>

#include "src/common/writefile.h"
#include "src/common/readfile.h"

#include "src/awe/determinetype.h"
#include "src/awe/cidfile.h"
#include "src/awe/dpfile.h"
#include "src/awe/objectxmlwritestream.h"

int main(int argc, char **argv) {
	CLI::App app("Convert cid files to readable xml files", "cid2xml");

	std::string cidFile, dpFile, bytecodeFile, bytecodeParametersFile;

	app.add_option("--bytecode", bytecodeFile, "The file containing byte code")
			->check(CLI::ExistingFile);
	app.add_option("--bytecode-parameters", bytecodeParametersFile, "The file containing parameters for bytecode")
			->check(CLI::ExistingFile);

	app.add_option("--dpfile", dpFile, "The dp file to load additional data")
		->check(CLI::ExistingFile);

	app.add_option("cidfile", cidFile, "The cidfile to be converted")
		->check(CLI::ExistingFile)
		->required();

	CLI11_PARSE(app, argc, argv);

	// Open cid file as stream
	Common::ReadFile cidFileStream(cidFile);

	// Initialize xml object
	Common::XML xml;
	auto &rootNode = xml.getRootNode();
	rootNode.name = "cid";

	// Generate stem and deduce object type from it
	const std::string stem = std::filesystem::path(cidFile).stem().string();
	ObjectType type = AWE::determineObjectTypeByFilename(stem);

	// Load dp file if given
	std::shared_ptr<DPFile> dp;
	if (!dpFile.empty())
		dp = std::make_shared<DPFile>(new Common::ReadFile(dpFile));

	AWE::ObjectXMLWriteStream xmlWriteStream(xml.getRootNode());

	// If byte code and bytecode parameters are given, create a bytecode collection
	if (!bytecodeFile.empty() && !bytecodeParametersFile.empty()) {
		std::shared_ptr<AWE::Script::Collection> collection = std::make_shared<AWE::Script::Collection>(
				new Common::ReadFile(bytecodeFile),
				new Common::ReadFile(bytecodeParametersFile)
		);
		xmlWriteStream.setBytecodeCollection(collection);
	}

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