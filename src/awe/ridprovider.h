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

#ifndef AWE_RIDPROVIDER_H
#define AWE_RIDPROVIDER_H

#include <map>
#include <string>

#include "cidfile.h"
#include "types.h"

/*!
 * \brief Base class for providing rid to name associations
 *
 * Abstract base class for providing rid to name associations
 * from different file formats. This associations create a link
 * between a rid, a 32 bit integer to a specific file path of
 * a resource.
 */
class RIDProvider {
public:
	/*!
	 * Return a name associated with the specified rid
	 * or an empty string if no name exists.
	 *
	 * \param rid the rid to test
	 * \return the associated name
	 */
	std::string getNameByRid(rid_t rid);

protected:
	std::map<rid_t, std::string> _resources;
	std::map<rid_t, std::vector<AWE::CIDFile::Container>> _metadata;
};


#endif //AWE_RIDPROVIDER_H
