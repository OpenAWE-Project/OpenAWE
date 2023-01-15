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

#ifndef OPENAWE_BITSTREAM_H
#define OPENAWE_BITSTREAM_H

#include <bit>

#include "src/common/readstream.h"
#include "src/common/endianness.h"

namespace Common {

/*!
 * \brief Class implementing a stream operating on bits
 *
 * This class offers the possiblity to read bits over a normal bitstream.
 *
 * A value is being read from a normal readstream depending on the template paremeter given and is then read bit by bit
 * until we have all requested bits for the value.
 *
 * \tparam T The data type on which the bitstream should operate
 * \tparam le If the bitstream read in little endian
 * \tparam msb If the stream is read with the most significant or least significant bit first
 */
template<typename T, bool le, bool msb>
class BitStream {
public:
	explicit BitStream(Common::ReadStream &bits) : _curValue(0), _remainingBits(0), _bits(bits) {}

	uint32_t read(size_t n) {
		uint32_t newValue = 0;
		if constexpr (msb)
			for (size_t i = 0; i < n; ++i) {
				newValue = (read() ? 1 : 0) | (newValue << 1);
			}
		else
			for (size_t i = 0; i < n; ++i) {
				newValue |= (read() ? 1 : 0) << i;
			}

		return newValue;
	}

	void skip(size_t n) {
		for (size_t i = 0; i < n; ++i) {
			read();
		}
	}

	bool read() {
		if (_remainingBits == 0)
			readNextValue();

		bool value;
		if constexpr (msb) {
			value = _curValue & (1 << (sizeof(T) * 8 - 1));
			_curValue <<= 1;
		} else {
			value = _curValue & 1;
			_curValue >>= 1;
		}

		_remainingBits--;
		return value;
	}

private:
	void readNextValue() {
		_bits.read(&_curValue, sizeof(T));
#ifdef LITTLE_ENDIAN_SYSTEM
		if constexpr (!le && sizeof(T) > 1)
			_curValue = swapBytes(_curValue);
#endif // LITTLE_ENDIAN_SYSTEM
#ifdef BIG_ENDIAN_SYSTEM
		if constexpr (le && sizeof(T) > 1)
			_curValue = swapBytes(_curValue);
#endif // BIG_ENDIAN_SYSTEM
		_remainingBits = sizeof(T) * 8;
	}

	T _curValue;
	unsigned short _remainingBits;
	Common::ReadStream &_bits;
};

typedef BitStream<uint8_t, false, true>   BitStream8MSB;
typedef BitStream<uint8_t, false, false>  BitStream8LSB;

typedef BitStream<uint16_t, true, true>   BitStream16LEMSB;
typedef BitStream<uint16_t, true, false>  BitStream16LELSB;
typedef BitStream<uint16_t, false, true>  BitStream16BEMSB;
typedef BitStream<uint16_t, false, false> BitStream16BELSB;

typedef BitStream<uint32_t, true, true>   BitStream32LEMSB;
typedef BitStream<uint32_t, true, false>  BitStream32LELSB;
typedef BitStream<uint32_t, false, true>  BitStream32BEMSB;
typedef BitStream<uint32_t, false, false> BitStream32BELSB;

typedef BitStream<uint64_t, true, true>   BitStream64LEMSB;
typedef BitStream<uint64_t, true, false>  BitStream64LELSB;
typedef BitStream<uint64_t, false, true>  BitStream64BEMSB;
typedef BitStream<uint64_t, false, false> BitStream64BELSB;

} // End of namespace Common

#endif //OPENAWE_BITSTREAM_H
