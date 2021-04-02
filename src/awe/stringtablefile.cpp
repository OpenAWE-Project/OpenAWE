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

#include "stringtablefile.h"

namespace AWE {

StringTableFile::StringTableFile(Common::ReadStream &stringTable) {
	uint32_t numElements = stringTable.readUint32LE();

	for (int i = 0; i < numElements; ++i) {
		uint32_t idLength = stringTable.readUint32LE();
		std::string id = stringTable.readFixedSizeString(idLength);

		uint32_t stringLength = stringTable.readUint32LE();
		std::u16string string = stringTable.readFixedSizeStringUTF16(stringLength);

		_strings[id] = string;
	}
}

std::u16string StringTableFile::get(const std::string &id) {
	return _strings[id];
}

}
