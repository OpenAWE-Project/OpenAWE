//
// Created by patrick on 23.11.23.
//

#include <cstdlib>

#include <filesystem>

#include <CLI/CLI.hpp>

#include "src/common/readfile.h"
#include "src/common/writefile.h"

#include "src/awe/rmdblobarchive.h"

int main(int argc, char** argv) {
	CLI::App app("Unpack rmdblob/rmdtoc archive structure", "unrmdblob");

	std::string rmdtocFile;

	app.add_option("rmdtoc", rmdtocFile, "The rmdtoc file containing the archives metadata")
			->check(CLI::ExistingFile)
			->required();

	CLI11_PARSE(app, argc, argv);

	Common::ReadFile rmdtoc(rmdtocFile);

	AWE::RMDBlobArchive rmdblob(rmdtoc, std::filesystem::path(rmdtocFile).parent_path());

	for (size_t i = 0; i < rmdblob.getNumResources(); ++i) {
		const std::string path = rmdblob.getResourcePath(i);

		fmt::print("{}/{} {}\n", i + 1, rmdblob.getNumResources(), path);

		const auto resourceStream = std::unique_ptr<Common::ReadStream>(rmdblob.getResource(path));

		std::filesystem::path p(path);
		if (!p.parent_path().empty())
			std::filesystem::create_directories(p.parent_path());

		Common::WriteFile writeFile(p.string());
		writeFile.writeStream(resourceStream.get());
		writeFile.close();
	}

	return EXIT_SUCCESS;
}
