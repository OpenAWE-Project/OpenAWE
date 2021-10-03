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

#include "src/common/xml.h"

#include "worldfile.h"

WorldFile::WorldFile(Common::ReadStream &readStream) {
	Common::XML xml;
	xml.read(readStream);

	Common::XML::Node rootNode = xml.getRootNode();
	if (rootNode.name != "world")
		throw std::runtime_error("Invalid world file");

	_worldName = rootNode.getString("name");

	for (auto &child : rootNode.children) {
		if (child.name != "episode")
			continue;

		readEpisode(child);
	}
}

void WorldFile::readEpisode(Common::XML::Node &node) {
	Episode episode;

	episode.number = std::stoul(node.properties["number"]);
	episode.name = node.properties["name"];

	for (auto &child : node.children) {
		if (child.name != "level")
			continue;

		Level level;

		level.name = child.getString("name");
		level.fileName = child.getString("filename");
		level.number = child.getInt("number");
		level.presence = child.getInt("presence");

		std::string startupVideo1 = child.getString("startupVideo");
		std::string startupVideo2 = child.getString("startupVideo2");
		std::string startupVideo3 = child.getString("startupVideo3");

		if (!startupVideo1.empty())
			level.startupVideos.emplace_back(startupVideo1);
		if (!startupVideo2.empty())
			level.startupVideos.emplace_back(startupVideo2);
		if (!startupVideo3.empty())
			level.startupVideos.emplace_back(startupVideo3);

		for (auto &item : child.children) {
			if (item.name != "lv3")
				continue;

			level.fileNames.emplace_back(item.getString("filename"));
		}

		episode.levels.emplace_back(level);
	}

	_episodes.emplace_back(episode);
}

const std::string &WorldFile::getWorldName() {
	return _worldName;
}

const WorldFile::Level & WorldFile::getLevel(const std::string &id) {
	for (const auto &episode : _episodes) {
		for (const auto &level : episode.levels) {
			if (level.fileName == id)
				return level;
		}
	}

	throw std::runtime_error("Level not found");
}
