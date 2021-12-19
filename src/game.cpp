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

#include <filesystem>
#include <memory>

#include <CLI/CLI.hpp>

#include <src/graphics/fontman.h>
#include "src/graphics/text.h"

#include "src/common/threadpool.h"
#include "src/common/strutil.h"
#include "src/common/exception.h"

#include "src/physics/physicsman.h"

#include "src/world.h"
#include "src/awe/resman.h"
#include "src/game.h"
#include "src/awe/cidfile.h"
#include "src/awe/havokfile.h"

#include "src/engines/aw/engine.h"
#include "src/engines/awan/engine.h"

#include "src/sound/soundman.h"

#include "src/task.h"

bool Game::parseArguments(int argc, char **argv) {
	CLI::App app("Reimplmentation of the Alan Wake Engine", "awe");

	unsigned int debugLevel;

	app.add_option("-p,--path", _path, "The path where the game data resides")
		->check(CLI::ExistingDirectory)
		->required();
	app.add_option("-d,--debug", debugLevel, "The debug level to use")
		->check(CLI::Range(0,6))
		->default_val(4);
	_physicsDebugDraw = false;
	app.add_flag("--debug-physics", _physicsDebugDraw, "Draw physics bodies for debugging");

	CLI11_PARSE(app, argc, argv);

	spdlog::set_level(spdlog::level::level_enum(6 - debugLevel));

	return true;
}

void Game::init() {
	spdlog::info("Initializing AWE...");

	if (_path.empty()) {
		spdlog::warn("No data path given, using current working directory");
		_path = "./";
	}

	std::vector<std::string> identifiers;

	// Index rmdp archives
	for (const auto &path : std::filesystem::directory_iterator(_path)) {
		if (!path.is_regular_file())
			continue;

		if (!std::regex_match(path.path().string(), std::regex(".+\\.rmdp$")))
			continue;

		std::string rmdpFile = path.path().string();
		std::string binFile = std::regex_replace(rmdpFile, std::regex("\\.rmdp$"), ".bin");

		identifiers.emplace_back(path.path().filename().stem().string());

		spdlog::info("Indexing archive {}", path.path().filename().string());
		//Threads.add([=](){ResMan.indexArchive(binFile, rmdpFile);});
		ResMan.indexArchive(binFile, rmdpFile);
	}

	if (identifiers.empty())
		throw std::runtime_error("No .rmdp files found in the set data path, exiting ...");

	/*ResMan.indexArchive(_path + "/ep999-000.bin", _path + "/ep999-000.rmdp");
	identifiers.emplace_back("ep999-000");*/

	//Common::ReadStream *s = ResMan.getResource("skeletons/female_skeleton.binhkx");
	//Common::ReadStream *s = ResMan.getResource("animations/emmaxx/ingame/emmaxx_stand.binhkt");
	//HavokFile female_skeleton(*s);

	GameEngine engine;

	// Check if the resources have packmeta files and load them and if not load streamed resources
	bool hasPackmeta = ResMan.hasResource("ep999-000.packmeta");
	if (hasPackmeta) {
		for (const auto &identifier : identifiers) {
			spdlog::info("Indexing packmeta file {}", identifier + ".packmeta");
			ResMan.indexPackmeta(identifier + ".packmeta");
		}
		engine = kAlanWakesAmericanNightmare;
	} else {
		spdlog::info("Indexing streamed resource file cid_streamedcloth.bin");
		ResMan.indexStreamedResource("resourcedb/cid_streamedcloth.bin");
		spdlog::info("Indexing streamed resource file cid_streamedcollisionpackage.bin");
		ResMan.indexStreamedResource("resourcedb/cid_streamedcollisionpackage.bin");
		spdlog::info("Indexing streamed resource file cid_streamedfacefxactor.bin");
		ResMan.indexStreamedResource("resourcedb/cid_streamedfacefxactor.bin");
		spdlog::info("Indexing streamed resource file cid_streamedfacefxanimset.bin");
		ResMan.indexStreamedResource("resourcedb/cid_streamedfacefxanimset.bin");
		spdlog::info("Indexing streamed resource file cid_streamedfoliagemesh.bin");
		ResMan.indexStreamedResource("resourcedb/cid_streamedfoliagemesh.bin");
		spdlog::info("Indexing streamed resource file cid_streamedhavokanimation.bin");
		ResMan.indexStreamedResource("resourcedb/cid_streamedhavokanimation.bin");
		spdlog::info("Indexing streamed resource file cid_streamedmesh.bin");
		ResMan.indexStreamedResource("resourcedb/cid_streamedmesh.bin");
		spdlog::info("Indexing streamed resource file cid_streamedparticlesystem.bin");
		ResMan.indexStreamedResource("resourcedb/cid_streamedparticlesystem.bin");
		spdlog::info("Indexing streamed resource file cid_streamedsound.bin");
		ResMan.indexStreamedResource("resourcedb/cid_streamedsound.bin");
		spdlog::info("Indexing streamed resource file cid_streamedtexture.bin");
		ResMan.indexStreamedResource("resourcedb/cid_streamedtexture.bin");

		engine = kAlanWake;
	}

	switch (engine) {
		case kAlanWake:
			_engine = std::make_unique<Engines::AlanWake::Engine>(_registry);
			break;
		case kAlanWakesAmericanNightmare:
			_engine = std::make_unique<Engines::AlanWakesAmericanNightmare::Engine>(_registry);
			break;
		default:
			throw Common::Exception("Game engine not recognized");
	}

	_platform.init();

	_window = std::make_unique<Graphics::Window>(Graphics::Window::kOpenGL);

	GfxMan.initOpenGL(*_window);
	//GfxMan.setAmbianceState("scene1_reststop_creepy");

	// Initialize Physics
	PhysicsMan.setDebugDraw(_physicsDebugDraw);

	// Initialize sound
	spdlog::info("Initializing sound system");
	SoundMan.init();

	// Initialize fonts
	spdlog::info("Loading font fixedsys");
	FontMan.load("fonts/fixedsys.binfnt", "fixedsys");

	_global = std::make_unique<Global>(_registry);

	switch (engine) {
		case kAlanWake:
			_engine = std::make_unique<Engines::AlanWake::Engine>(_registry);
			break;
		case kAlanWakesAmericanNightmare:
			_engine = std::make_unique<Engines::AlanWakesAmericanNightmare::Engine>(_registry);
			break;
		default:
			throw Common::Exception("Game engine not recognized");
	}
	_window->setTitle(_engine->getName());

	_engine->init();
}

void Game::start() {
	spdlog::info("Starting AWE...");

	glm::vec3 cameraPosition(0.0f, 500.0f,0.0f);
	glm::vec3 cameraDirection(0.0f, 0.0f, -1.0f);

	Graphics::Camera camera;
	camera.setPosition(cameraPosition);
	camera.setDirection(cameraDirection);

	Graphics::Text text;
	text.setText(u"OpenAWE - v0.0.1");
	text.show();

	bool forward = false, backward = false, left = false, right = false, up = false, down = false, turnLeft = false, turnRight = false;
	_window->setKeyCallback([&](int key, int scancode, int action, int mods){
		if (key == GLFW_KEY_W)
			forward = action == GLFW_PRESS || action == GLFW_REPEAT;
		else if (key == GLFW_KEY_S)
			backward = action == GLFW_PRESS || action == GLFW_REPEAT;
		else if (key == GLFW_KEY_A)
			left = action == GLFW_PRESS || action == GLFW_REPEAT;
		else if (key == GLFW_KEY_D)
			right = action == GLFW_PRESS || action == GLFW_REPEAT;
		else if (key == GLFW_KEY_R)
			up = action == GLFW_PRESS || action == GLFW_REPEAT;
		else if (key == GLFW_KEY_F)
			down = action == GLFW_PRESS || action == GLFW_REPEAT;
		else if (key == GLFW_KEY_Q)
			turnLeft = action == GLFW_PRESS || action == GLFW_REPEAT;
		else if (key == GLFW_KEY_E)
			turnRight = action == GLFW_PRESS || action == GLFW_REPEAT;
	});

	bool exit = false;
	std::chrono::system_clock::time_point last, now;
	while (!exit) {
		GfxMan.drawFrame();

		if (forward)
			cameraPosition.z -= 5.0f;
		if (backward)
			cameraPosition.z += 5.0f;
		if (left)
			cameraPosition.x -= 5.0f;
		if (right)
			cameraPosition.x += 5.0f;
		if (up)
			cameraPosition.y -= 5.0f;
		if (down)
			cameraPosition.y += 5.0f;

		if (forward || backward || left || right || up || down) {
			camera.setPosition(cameraPosition);
			GfxMan.setCamera(camera);
		}

		PhysicsMan.update(0.0f); // TODO

		_platform.update();
		if (_window->shouldClose())
			exit = true;
	}

	spdlog::info("Stopping AWE...");

	_engine->getConfiguration().write();

	_platform.terminate();
}
