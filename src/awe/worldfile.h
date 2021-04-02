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

#ifndef AWE_WORLDFILE_H
#define AWE_WORLDFILE_H

#include <vector>

#include "src/common/readstream.h"
#include "src/common/xml.h"

/*!
 * \brief class for reading xml files describing worlds.
 *
 * This class is responsible for extracting vital information
 * from a world xml file, extracting name, levels and episodes
 * and their appropriate attributes.
 */
class WorldFile {
public:
	struct Level {
		unsigned int number;
		unsigned int presence;
		std::string name;
		std::string fileName;
		std::vector<std::string> startupVideos;
		std::vector<std::string> fileNames;
	};

	struct Episode {
		unsigned int number;
		std::string name;
		std::vector<Level> levels;
	};

	explicit WorldFile(Common::ReadStream &readStream);

	const std::string &getWorldName();
	const Level & getLevel(const std::string &id);

private:
	void readEpisode(Common::XML::Node &node);

	std::string _worldName;
	std::vector<Episode> _episodes;
};


#endif //AWE_WORLDFILE_H
