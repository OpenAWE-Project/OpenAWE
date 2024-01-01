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

#ifndef OPENAWE_PROCADDRESS_H
#define OPENAWE_PROCADDRESS_H

namespace Platform {

/*!
 * Get the proc address of a specific gl function. A context must be made current before using this function, otherwise
 * the behaviour is undefined.
 *
 * \param name The name of the opengl function
 * \return The proc address of the function or null if the retrieval failed
 */
void *getProcAddressGL(const char *name);

} // End of namespace Platform

#endif //OPENAWE_PROCADDRESS_H
