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

#ifndef SRC_COMMON_READFILE_H
#define SRC_COMMON_READFILE_H

#include <string>
#include <fstream>

#include "src/common/readstream.h"

namespace Common {

/*!
 * \brief Class for accessing files as a ReadStream
 *
 * This class offers the possibility to access a file using a ReadStream. It offers the common methods which the base
 * ReadStream class accesses.
 */
class ReadFile : public ReadStream {
public:
	/*!
	 * Construct a file readstream from a
	 * given file
	 * \param file the file to read
	 */
	ReadFile(const std::string &file);

	/*!
	 * Read a generic chunk of data
	 * \param data the data buffer to read
	 * \param length the length to read
	 */
	size_t read(void *data, size_t length) override;

	/*!
	 * Get the current position in the file
	 * \return The current position  in the file
	 */
	size_t pos() const override;

	/*!
	 * If we are at the end of the file or beyond it
	 * \return If the current position in the file is at the end or beyond it
	 */
	bool eos() const override;

	/*!
	 * Seek in the file according to a certain origin
	 * \param length The amount to seek from the origin. Can be negative too
	 * \param origin The origin from which to seek. Can be the begin and end of the file or the current position
	 */
	void seek(ptrdiff_t length, SeekOrigin origin) override;

	size_t size() const override;

private:
	size_t _fileSize;
	mutable std::ifstream _in;
};

} // End of namespace Common

#endif // SRC_COMMON_READFILE_H
