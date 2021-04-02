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

#ifndef AWE_ARCHIVE_H
#define AWE_ARCHIVE_H

#include "src/common/readstream.h"

namespace AWE {

/*!
 * \brief Abstract base class for archives
 *
 * This class serves as a base for the archive classes
 * of bin/rmdp archive structures and bin archives. It
 * offers common access methods for accessing the files
 * used by these archives
 */
class Archive : Common::Noncopyable {
public:
	/*!
	 * Get the number of resources contained in this archive
	 *
	 * \return the number of resources in this archive
	 */
	virtual size_t getNumResources() = 0;

	/*!
	 * Try to get a resource using the virtual path to it.
	 *
	 * \param rid the path to search for
	 * \return The found resource or NULL if the resource
	 * is not available
	 */
	virtual Common::ReadStream *getResource(const std::string &rid) const = 0;

	/*!
	 * Check if a resource given by a path exists.
	 *
	 * \param rid the path to search for
	 * \return if the resource specified by rid exists
	 */
	virtual bool hasResource(const std::string &rid) const = 0;
};

} // End of namespace AWE

#endif //AWE_ARCHIVE_H
