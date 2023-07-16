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

#include <fmt/format.h>

#if HAS_CPUID_H
#	include <cpuid.h>
#endif

#if HAS_GETAUXVAL
#   include <sys/auxv.h>
#   define HWCAP_NEON (1 << 12)
#endif

#include "src/common/cpuinfo.h"
#include "src/common/strutil.h"
#include "src/common/readfile.h"

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

std::string getARMVendorName(unsigned int id) {
	switch (id) {
		case 0x41: return "ARM";
		case 0x42: return "Broadcom";
		case 0x43: return "Cavium";
		case 0x44: return "DEC";
		case 0x46: return "Fujitsu";
		case 0x48: return "HiSilicon";
		case 0x49: return "Infineon";
		case 0x4D: return "Motorola/Freescale";
		case 0x4E: return "Nvidia";
		case 0x50: return "APM";
		case 0x51: return "Qualcomm";
		case 0x53: return "Samsung";
		case 0x54: return "Texas Instruments";
		case 0x56: return "Marvell";
		case 0x61: return "Apple";
		case 0x66: return "Faraday";
		case 0x69: return "Intel";
		case 0x70: return "Phytium";
		case 0xC0: return "Ampere";
		default:
			return fmt::format("Unknown Vendor 0x{:x}", id);
	}
}

std::string getCPUInfoParameter(const std::string key) {
	Common::ReadFile cpuinfo("/proc/cpuinfo");

	const auto keyRegex = std::regex(fmt::format("{}\t*: .*", key));

	while (!cpuinfo.eos()) {
		const auto cpuinfoLine = cpuinfo.readLine();

		if (std::regex_match(cpuinfoLine, keyRegex)) {
			const auto splitLine = Common::split(cpuinfoLine, std::regex(": "));
			const auto key = splitLine[0];
			const auto value = splitLine[1];

			return value;
		}
	}

	return "";
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
#elif ARCH_ARM && OS_LINUX
	const auto implementerID = std::stoi(getCPUInfoParameter("CPU implementer"), 0, 16);
	return getARMVendorName(implementerID);
#else
	return "<unknown>";
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
	return "<unknown>";
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
