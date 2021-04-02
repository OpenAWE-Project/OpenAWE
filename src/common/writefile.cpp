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

#include "writefile.h"

namespace Common {

WriteFile::WriteFile(const std::string &file) : _out(file, std::ios::out | std::ios::binary) {
}

WriteFile::~WriteFile() {
	close();
}

void WriteFile::write(const void *data, size_t length) {
	_out.write(reinterpret_cast<const char *>(data), length);
}

void WriteFile::flush() {
	_out.flush();
}

void WriteFile::close() {
	_out.close();
}

void WriteFile::seek(ptrdiff_t length, WriteStream::SeekOrigin origin) {
	switch (origin) {
		case BEGIN:
			_out.seekp(length, std::ios::beg);
			break;
		case CURRENT:
			_out.seekp(length, std::ios::cur);
			break;
		case END:
			_out.seekp(length, std::ios::end);
			break;
	}
}

size_t WriteFile::pos() {
	return _out.tellp();
}

}
