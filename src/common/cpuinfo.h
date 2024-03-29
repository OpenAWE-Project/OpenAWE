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

#ifndef OPENAWE_CPUINFO_H
#define OPENAWE_CPUINFO_H

#include <string>

namespace Common {

/*!
 * Get the vendor of the executed cpu using the cpuid opcode. If the vendor retrieval is not possible or supported,
 * return an empty string.
 * \return The current cpu vendor
 */
std::string getCPUVendor();

/*!
 * Get the name of the current cpu or <unknown> if it is not recognizable
 * \return The name of the current cpu
 */
std::string getCPUName();

/*!
 * Get the name of the operating system currently running
 * \return The name of the operating system
 */
std::string getOSName();

/*!
 * Return if the cpu supports MMX instructions
 * \return If the cpu supports MMX instructions
 */
bool hasMMX();

/*!
 * Return if the cpu supports SSE instructions
 * \return If the cpu supports SSE instructions
 */
bool hasSSE();

/*!
 * Return if the cpu supports SSE2 instructions
 * \return If the cpu supports SSE2 instructions
 */
bool hasSSE2();

/*!
 * Return if the cpu supports SSE3 instructions
 * \return If the cpu supports SSE3 instructions
 */
bool hasSSE3();

/*!
 * Return if the cpu supports supplemental SSE3 instructions
 * \return If the cpu supports supplemental SSE3 instructions
 */
bool hasSSSE3();

/*!
 * Return if the cpu supports NEON instructions
 * \return If the cpu supports NEON instructions
 */
bool hasNEON();

} // End of namespace Common

#endif //OPENAWE_CPUINFO_H
