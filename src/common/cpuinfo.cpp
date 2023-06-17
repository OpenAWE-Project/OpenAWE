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

#include <cstring>
#include <cstdint>

#if HAS_CPUID_H
#	include <cpuid.h>
#endif

#if HAS_GETAUXVAL
#   include <sys/auxv.h>
#   define HWCAP_NEON (1 << 12)
#endif

#include "src/common/cpuinfo.h"
#include "src/common/strutil.h"

static bool cpuinfoRetrieved = false;

static bool has_mmx   = false;
static bool has_sse   = false;
static bool has_sse2  = false;
static bool has_sse3  = false;
static bool has_ssse3 = false;

void retrieveCPUInfo() {
	if (cpuinfoRetrieved)
		return;

	cpuinfoRetrieved = true;

#if HAS_CPUID_H
	uint32_t data[4];

	__cpuid(1, data[0], data[1], data[2], data[3]);

	has_mmx   = data[3] & bit_MMX;
	has_sse   = data[3] & bit_SSE;
	has_sse2  = data[3] & bit_SSE2;
	has_sse3  = data[2] & bit_SSE3;
	has_ssse3 = data[2] & bit_SSSE3;
#endif
}

namespace Common {

std::string getCPUVendor() {
#if HAS_CPUID_H
	uint32_t data[4];

	__cpuid(0, data[0], data[1], data[2], data[3]);

	std::string name;
	name.resize(12);

	std::memcpy(name.data(), &data[1], 4);
	std::memcpy(name.data() + 4, &data[3], 4);
	std::memcpy(name.data() + 8, &data[2], 4);

	return name;
#else
	return "";
#endif
}

std::string getCPUName() {
#if HAS_CPUID_H
	uint32_t data[12];

	__cpuid(0x80000002, data[0], data[1], data[2], data[3]);
	__cpuid(0x80000003, data[4], data[5], data[6], data[7]);
	__cpuid(0x80000004, data[8], data[9], data[10], data[11]);

	std::string name(reinterpret_cast<char*>(data));

	return name;
#else
	return "";
#endif
}

bool hasMMX() {
	retrieveCPUInfo();
	return has_mmx;
}

bool hasSSE() {
	retrieveCPUInfo();
	return has_sse;
}

bool hasSSE2() {
	retrieveCPUInfo();
	return has_sse2;
}

bool hasSSE3() {
	retrieveCPUInfo();
	return has_sse3;
}

bool hasSSSE3() {
	retrieveCPUInfo();
	return has_ssse3;
}

bool hasNEON() {
#if HAS_GETAUXVAL
    return (getauxval(AT_HWCAP) & HWCAP_NEON) == HWCAP_NEON;
#else
    return false;
#endif
}

}
