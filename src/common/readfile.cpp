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
#include <assert.h>

#include "src/common/readfile.h"
#include "src/common/exception.h"

namespace Common {

ReadFile::ReadFile(const std::string &file) :
	_fileSize(std::filesystem::file_size(file)),
	_in(file.c_str(), std::ios::in | std::ios::binary) {
	if (!std::filesystem::is_regular_file(file))
		throw Common::Exception("{} not found", file);
}

size_t ReadFile::read(void *data, size_t length) {
	_in.read(reinterpret_cast<char *>(data), length);

	return _in.gcount();
}

void ReadFile::seek(ptrdiff_t length, ReadStream::SeekOrigin origin) {
	switch (origin) {
		case BEGIN:
			_in.seekg(length, std::ios::beg);
			break;
		case CURRENT:
			_in.seekg(length, std::ios::cur);
			break;
		case END:
			_in.seekg(length, std::ios::end);
			break;
	}
}

bool ReadFile::eos() const {
	return _in.peek() == EOF;
}

size_t ReadFile::pos() const {
    if (_in.fail())
        throw Common::Exception("File stream failed");

    const auto pos = _in.tellg();
    if (pos == std::istream::pos_type(-1))
		throw Common::Exception("Invalid file stream position");

	return _in.tellg();
}

size_t ReadFile::size() const {
	return _fileSize;
}

}
