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

#include <string>
#include <filesystem>

#include <spdlog/spdlog.h>

#include "src/common/writefile.h"
#include "src/common/readfile.h"
#include "src/common/platform.h"
#include "src/common/xml.h"

#include "configuration.h"

namespace Engines::AlanWakesAmericanNightmare {

Configuration::Configuration() :
	_brightness(0.5),
	_subtitles(false),
	_securityArea(0.5),
	_musicVolume(1.0),
	_effectVolume(1.0),
	_speechVolume(1.0),
	_movieVolume(1.0),
	_horicontallyInverted(false),
	_verticallyInverted(false),
	_sensitivity(1.0),
	_hapticFeedback(false),
	_profileName("default") {
}

void Configuration::write() {
	spdlog::info("Writing Alan Wakes American Nightmare configuration");

	std::string path = std::format("{}/openawe/awan", Common::getUserDataDirectory());
	if (!std::filesystem::is_directory(path))
		std::filesystem::create_directories(path);

	std::string configFile = std::format("{}/config", path);
	std::string resolutionFile = std::format("{}/resolution.xml", path);

	auto resolutionStream = std::make_unique<Common::WriteFile>(resolutionFile);
	auto configStream = std::make_unique<Common::WriteFile>(configFile);

	writeResolution(*resolutionStream);
	resolutionStream->close();

	writeConfiguration(*configStream);
	configStream->close();
}

void Configuration::read() {
	spdlog::info("Reading Alan Wakes American Nightmare configuration");

	std::string path = std::format("{}/openawe/awan", Common::getUserDataDirectory());
	if (!std::filesystem::is_directory(path))
		std::filesystem::create_directories(path);

	std::string configFile = std::format("{}/config", path);
	std::string resolutionFile = std::format("{}/resolution.xml", path);

	if (std::filesystem::is_regular_file(resolutionFile)) {
		auto resolutionStream = std::make_unique<Common::ReadFile>(resolutionFile);
		readResolution(*resolutionStream);
	}

	if (std::filesystem::is_regular_file(configFile)) {
		auto configStream = std::make_unique<Common::ReadFile>(configFile);
		readConfiguration(*configStream);
	}

	// TODO: If the configuration file is not found fill it with standard values or try to determine good values for the system and write it
}

void Configuration::readResolution(Common::ReadStream &file) {
	Common::XML xml;
	xml.read(file);
	auto &rootNode = xml.getRootNode();

	if (rootNode.name != "screen_resolution")
		throw std::runtime_error("Invalid resolution file");

	for (auto &child : rootNode.children) {
		if (child.name == "width")
			resolution.width = std::stoi(child.properties["value"]);
		else if (child.name == "height")
			resolution.height = std::stoi(child.properties["value"]);
		else if (child.name == "fullscreen")
			resolution.fullscreen = std::stoi(child.properties["value"]) == 1;
		else
			throw std::runtime_error("Invalid resolution tag");
	}
}

void Configuration::readConfiguration(Common::ReadStream &file) {
	uint32_t magicNumber = file.readUint32LE();
	if (magicNumber != 150)
		throw std::runtime_error("Invalid magic number for configuration");

	file.skip(1);
	_brightness = file.readIEEEFloatLE();
	_subtitles = file.readByte() != 0;
	file.skip(8);
	_securityArea = file.readIEEEFloatLE();
	_musicVolume = file.readIEEEFloatLE();
	_effectVolume = file.readIEEEFloatLE();
	_speechVolume = file.readIEEEFloatLE();
	_movieVolume = file.readIEEEFloatLE();
	_horicontallyInverted = file.readByte() != 0;
	_verticallyInverted = file.readByte() != 0;
	_sensitivity = file.readIEEEFloatLE();
	_hapticFeedback = file.readByte() != 0;
}

void Configuration::writeResolution(Common::WriteStream &file) {
	Common::XML xml;
	Common::XML::Node &rootNode = xml.getRootNode();
	rootNode.name = "screen_resolution";

	Common::XML::Node &widthNode = rootNode.children.emplace_back();
	widthNode.name = "width";
	widthNode.properties["value"] = std::to_string(resolution.width);

	Common::XML::Node &heightNode = rootNode.children.emplace_back();
	heightNode.name = "height";
	heightNode.properties["value"] = std::to_string(resolution.height);

	Common::XML::Node &fullscreenNode = rootNode.children.emplace_back();
	fullscreenNode.name = "fullscreen";
	fullscreenNode.properties["value"] = std::to_string(resolution.fullscreen);

	xml.write(file, false);
}

void Configuration::writeConfiguration(Common::WriteStream &file) {
	file.writeUint32LE(150); // Magic Number
	file.writeByte(false);
	file.writeIEEEFloatLE(_brightness);
	file.writeByte(_subtitles ? 1 : 0);
	file.writeIEEEFloatLE(0.0f);
	file.writeIEEEFloatLE(0.0f);
	file.writeIEEEFloatLE(_securityArea);
	file.writeIEEEFloatLE(_musicVolume);
	file.writeIEEEFloatLE(_effectVolume);
	file.writeIEEEFloatLE(_speechVolume);
	file.writeIEEEFloatLE(_movieVolume);
	file.writeByte(_horicontallyInverted ? 1 : 0);
	file.writeByte(_verticallyInverted ? 1 : 0);
	file.writeIEEEFloatLE(_sensitivity);
	file.writeByte(_hapticFeedback ? 1 : 0);
	file.writeUint32LE(0);

	file.writeUint32LE(_profileName.size());
	file.writeString(_profileName);

	file.writeByte(_manuscriptLevels.size());
	for (const auto &item : _manuscriptLevels)
		file.writeByte(item);
}

}
