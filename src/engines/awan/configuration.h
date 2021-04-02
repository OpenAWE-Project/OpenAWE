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

#ifndef OPENAWE_AWAN_CONFIGURATION_H
#define OPENAWE_AWAN_CONFIGURATION_H

#include <set>

#include "src/common/writestream.h"

#include "src/configuration.h"

namespace Engines::AlanWakesAmericanNightmare {

class Configuration : public ::Configuration {
public:
	Configuration();

private:
	float _brightness;

	bool _subtitles;

	float _securityArea;

	float _musicVolume;
	float _effectVolume;
	float _speechVolume;
	float _movieVolume;

	bool _horicontallyInverted;
	bool _verticallyInverted;
	float _sensitivity;
	bool _hapticFeedback;

	std::string _profileName;

	std::set<uint8_t> _manuscriptLevels;

	void write() override;
	void read() override;

	void readResolution(Common::ReadStream &file);
	void readConfiguration(Common::ReadStream &file);

	void writeResolution(Common::WriteStream &file);
	void writeConfiguration(Common::WriteStream &file);
};

}

#endif //OPENAWE_CONFIGURATION_H
