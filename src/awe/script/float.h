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

#ifndef OPENAWE_FLOAT_H
#define OPENAWE_FLOAT_H

#include <cstdint>

namespace AWE::Script {

/*!
 * Check if the the 32 bit value is probably a float value
 *
 * \note This function should only be used for logging purposes, since it cannot give exact results.
 * \param value The value to check
 * \return If this value is probably a float value
 */
bool isFloat(int32_t value);

/*!
 * Convert the value to a float value by reinterpreting it
 *
 * \param value The value to conver
 * \return The converted float value
 */
float asFloat(int32_t value);

} // End of namespace AWE::Script

#endif //OPENAWE_FLOAT_H
