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

#include "src/common/crc32.h"
#include "src/common/threadpool.h"
#include "src/common/strutil.h"
#include "src/common/exception.h"
#include "src/common/platform.h"
#include "src/common/cpuinfo.h"

#include "src/awe/resman.h"
#include "src/awe/cidfile.h"
#include "src/awe/havokfile.h"
#include "src/awe/types.h"

#include "src/platform/keyconversion.h"
#include "src/platform/gamepadconversion.h"
#include "src/platform/gamepadman.h"

#include "src/graphics/fontman.h"
#include "src/graphics/text.h"
#include "src/graphics/meshman.h"
#include "src/graphics/binmshloader.h"
#include "src/graphics/binfolloader.h"

#include "src/events/eventman.h"

#include "src/physics/physicsman.h"

#include "src/engines/aw/engine.h"
#include "src/engines/awan/engine.h"

#include "src/sound/soundman.h"

#include "src/game.h"
#include "src/controlledfreecamera.h"
#include "src/task.h"
#include "src/timerprocess.h"
#include "src/transform.h"
#include "src/probe.h"
#include "src/utils.h"
#include "src/world.h"

static constexpr uint32_t kLockMouse = Common::crc32("MOUSE_LOCK");

bool Game::parseArguments(int argc, char **argv) {
	CLI::App app("Reimplmentation of the Alan Wake Engine", "awe");

	unsigned int debugLevel;

	app.add_option("-p,--path", _path, "The path where the game data resides")
		->check(CLI::ExistingDirectory)
		->required();
	app.add_option("-d,--debug", debugLevel, "The debug level to use")
		->check(CLI::Range(0,6))
		->default_val(4);
	app.add_option("-l,--language", _language, "The language used to play the game")
		->transform(CLI::CheckedTransformer(std::map<std::string, Common::Language>{
			{"en", Common::kEnglish},
			{"ja", Common::kJapanese},
			{"de", Common::kGerman},
			{"fr", Common::kFrench},
			{"es", Common::kSpanish},
			{"it", Common::kItalian},
			{"ko", Common::kKorean},
			{"pl", Common::kPolish},
			{"zh", Common::kChinese},
			{"ru", Common::kRussian},
			{"cz", Common::kCzech},
			{"hu", Common::kHungarian}
		}, CLI::ignore_case))
		->default_val(Common::getSystemLanguage());
	app.add_option("-s,--shader-path", _shaderPath, "The path in which the shader files are stored")
		->default_val("../shaders");

	_physicsDebugDraw = false;
	app.add_flag("--debug-physics", _physicsDebugDraw, "Draw physics bodies for debugging");

	CLI11_PARSE(app, argc, argv);

	spdlog::set_level(spdlog::level::level_enum(6 - debugLevel));

	return true;
}

void Game::init() {
	spdlog::debug("OS: {}", Common::getOSName());
	spdlog::debug("CPU Vendor: {}", Common::getCPUVendor());
	spdlog::debug("CPU Name: {}",   Common::getCPUName());
	spdlog::debug("MMX: {}",    Common::hasMMX());
	spdlog::debug("SSE: {}",    Common::hasSSE());
	spdlog::debug("SSE2: {}",   Common::hasSSE2());
	spdlog::debug("SSE3: {}",   Common::hasSSE3());
	spdlog::debug("SSSE3: {}",  Common::hasSSSE3());
	spdlog::debug("NEON: {}",   Common::hasNEON());

	spdlog::info("Initializing AWE...");

	if (_path.empty()) {
		spdlog::warn("No data path given, using current working directory");
		_path = "./";
	}

	std::vector<std::string> identifiers;

	ResMan.setRootPath(_path);

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
		throw CreateException("No .rmdp files found in the set data path.");

	GameEngine engine;

	// Launch probe to see what game we are dealing with
	Probe probe = Probe();
	ProbeResult probeResult = probe.performProbe();
	switch (probeResult) {
		case kResultAlanWake:
			spdlog::info("Initializing Alan Wake...");
			engine = kAlanWake;
			break;
		case kResultAmericanNightmare:
			spdlog::info("Initializing Alan Wake's American Nightmare...");
			engine = kAlanWakesAmericanNightmare;
			break;
		case kResultQuantumBreak:
			throw CreateException("Quantum Break is not yet supported by OpenAWE.");
		case kResultAlanWakeRemastered:
			throw CreateException("Alan Wake Remastered is not yet supported by OpenAWE.");
		case kResultUnknown:
		default:
			throw CreateException("Unknown game data was supplied.");
	}

	// Check if the resources have packmeta files and load them and if not load streamed resources
	if (engine == kAlanWakesAmericanNightmare) {
		for (const auto &identifier : identifiers) {
			spdlog::info("Indexing packmeta file {}", identifier + ".packmeta");
			ResMan.indexPackmeta(identifier + ".packmeta");
		}
	} else if (engine == kAlanWake) {
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
	}

	_platform.init();

	_window = std::make_unique<Platform::Window>(Platform::Window::kOpenGL);


	GfxMan.initOpenGL(*_window, _shaderPath);
	//GfxMan.setAmbianceState("scene1_reststop_creepy");

	// Initialize loaders
	spdlog::info("Initializing mesh loaders");
	MeshMan.appendLoader<Graphics::BINMSHLoader>();
	MeshMan.appendLoader<Graphics::BINFOLLoader>();

	// Initialize Physics
	PhysicsMan.setDebugDraw(_physicsDebugDraw);

	// Initialize sound
	spdlog::info("Initializing sound system");
	SoundMan.init();

	// Initialize fonts
	spdlog::info("Loading font fixedsys");
	FontMan.load("fonts/fixedsys.binfnt", "fixedsys");

	// Initialize locale config
	std::unique_ptr<Common::ReadStream> localeConfigStream(ResMan.getResource("config/locale_config.xml"));
	LocaleConfig localeConfig(*localeConfigStream);

	_global = std::make_unique<Global>(_registry);

	switch (engine) {
		case kAlanWake:
			_engine = std::make_unique<Engines::AlanWake::Engine>(
				_registry,
				localeConfig.getLanguageConfig(_language)
			);
			break;
		case kAlanWakesAmericanNightmare:
			_engine = std::make_unique<Engines::AlanWakesAmericanNightmare::Engine>(
				_registry,
				localeConfig.getLanguageConfig(_language)
			);
			break;
		case kQuantumBreak:
			throw Common::Exception("This game is not yet supported by OpenAWE");
		default:
			throw Common::Exception("Game engine not recognized");
	}
	_window->setTitle(_engine->getName());

	_engine->init();
}

void Game::start() {
	spdlog::info("Starting AWE...");

	ControlledFreeCamera freeCamera;

	GfxMan.setCamera(freeCamera);

	Graphics::Text text;
	text.setText(u"OpenAWE - v0.0.1");
	text.show();

	// Allow locking and unlocking the mouse
	EventMan.setActionCallback({ kLockMouse }, [&](Events::Event event){
		Events::KeyEvent key = std::get<Events::KeyEvent>(event.data);
		if(key.state == Events::kPress) {
				_window->setMouseCursorVisible(!_window->isMouseCursorVisible());}});
	EventMan.addBinding(kLockMouse, Events::kKeyL, Events::kModifierAlt);

	_window->setKeyCallback([&](int key, int scancode, int action, int modifiers){
		EventMan.injectKeyboardInput(Platform::convertGLFW2Key(key), action == GLFW_RELEASE ? Events::kRelease : Events::kPress, modifiers);
	});

	_window->setMouseButtonCallback([&](int button, int action, int mods){
		EventMan.injectMouseButtonInput(static_cast<Events::MouseButton>(button), action == GLFW_RELEASE ? Events::kRelease : Events::kPress);
	});

	_window->setMousePositionCallback([&](glm::vec2 absolute, glm::vec2 delta){
		EventMan.injectMouse2DAxisInput(Events::kMousePosition, absolute, delta);
	});

	_window->setMouseScrollCallback([&](glm::vec2 absolute, glm::vec2 delta){
		EventMan.injectMouse1DAxisInput(Events::kMouseScrollHorizontal, absolute.x, delta.x);
		EventMan.injectMouse1DAxisInput(Events::kMouseScrollVertical, absolute.y, delta.y);
	});

	GamepadMan.setGamepadButtonCallback([&](int button, int action){
		EventMan.injectGamepadButtonInput(static_cast<Events::GamepadButton>(button), action == GLFW_RELEASE? Events::kRelease : Events::kPress);
	});

	GamepadMan.setGamepadStickCallback([&](int stick, glm::vec2 pos, glm::vec2 delta){
		EventMan.injectGamepad2DAxisInput(Platform::convertGLFW2GamepadStick(stick), pos, delta);
	});

	GamepadMan.setGamepadTriggerCallback([&](int trigger, double pos, double delta){
		EventMan.injectGamepad1DAxisInput(Platform::convertGLFW2GamepadTrigger(trigger), pos, delta);
	});

	entt::observer transformRelationshipObserver {
		_registry,
		entt::collector.update<Transform>().where<Relationship>()
	};
	entt::observer transformModelObserver{
		_registry,
		entt::collector.update<Transform>().where<Graphics::ModelPtr>()
	};
	entt::observer transformLightObserver{
		_registry,
		entt::collector.update<Transform>().where<Graphics::Light>()
	};

	double lastTime = _platform.getTime();
	bool exit = false;
	std::chrono::system_clock::time_point last, now;
	while (!exit) {
		double time = _platform.getTime();

		_engine->update(time);
		_engine->getScheduler().update(time);

		/*
		 * Update child transforms if a parent transform is changed
		 */
		for (const auto &transformEntity: transformRelationshipObserver) {
			const auto &transform = _registry.get<Transform>(transformEntity);
			const auto &relationship = _registry.get<Relationship>(transformEntity);

			for (const auto &child : relationship.children) {
				_registry.get<Transform>(child).setParentTransform(transform.getTransformation());

				// TODO: Go into the deep structure
			}
		}

		/*
		 * Change a models transform when the entities transform has changed
		 */
		for (const auto &transformEntity : transformModelObserver) {
			const Transform &transform = _registry.get<Transform>(transformEntity);
			Graphics::ModelPtr model = _registry.get<Graphics::ModelPtr>(transformEntity);

			model->setTransform(transform.getTransformation());
		}

		/*
		 * Change a lights transform when the entities transform has changed
		 */
		for (const auto &lightEntity: transformLightObserver) {
			const Transform &transform = _registry.get<Transform>(lightEntity);
			_registry.get<Graphics::Light>(lightEntity).setTransform(transform.getTransformation());
		}

		freeCamera.update(time - lastTime);

		PhysicsMan.update(time - lastTime);
		GfxMan.drawFrame();

		_platform.update();
		GamepadMan.pollGamepadEvents();
		if (_window->shouldClose())
			exit = true;

		lastTime = time;
	}

	transformRelationshipObserver.disconnect();
	transformModelObserver.disconnect();
	transformLightObserver.disconnect();

	spdlog::info("Stopping AWE...");

	_engine->writeConfiguration();

	_window.reset();
	_platform.terminate();
}
