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

#include <algorithm>
#include <cstring>

#if __SSE2__ || _M_IX86_FP==2
#	include <emmintrin.h>
#endif

#include "src/common/exception.h"

#include "src/codecs/yuv2rgb.h"

namespace Codecs {

void convertYUV2RGB(const YCbCrBuffer &ycbcr, byte *rgb, unsigned int width, unsigned int height) {
	for (unsigned int y = 0; y < height; ++y) {
		for (unsigned int x = 0; x < width; ++x) {
			const int y1 = ycbcr.y[y * width + x] << 6;
			const int cb = ycbcr.cb[(y / 2) * (width / 2) + x / 2];
			const int cr = ycbcr.cr[(y / 2) * (width / 2) + x / 2];

			const int r = (y1 + 90 * (cr - 128)) >> 6;
			const int g = (y1 - 22 * (cb - 128) - (cr - 128)) >> 6;
			const int b = (y1 + 114 * (cb - 128)) >> 6;

			rgb[(y * width + x) * 3]     = std::clamp(r, 0, 255);
			rgb[(y * width + x) * 3 + 1] = std::clamp(g, 0, 255);
			rgb[(y * width + x) * 3 + 2] = std::clamp(b, 0, 255);
		}
	}
}

void convertYUV2RGB_SSE2(const YCbCrBuffer &ycbcr, byte *rgb, unsigned int width, unsigned int height) {
#if __SSE2__ || _M_IX86_FP==2
	__m128i uvdiff = _mm_set1_epi16(128);

	__m128i fac_r_cr = _mm_set1_epi16(90);
	__m128i fac_g_cb = _mm_set1_epi16(22);
	__m128i fac_g_cr = _mm_set1_epi16(46);
	__m128i fac_b_cb = _mm_set1_epi16(114);
	__m128i zero = _mm_set1_epi16(0);

	__m128i
		y1[2], cr, cb, cr0, cr1, cb0, cb1,
		y2[4], r_cr[2], g_cb[2], g_cr[2], b_cb[2],
		r[2], g[2], b[2];
	byte result_r[16], result_g[16], result_b[16];

	assert(width % 16 == 0);
	assert(height % 2 == 0);

	for (unsigned int y = 0; y < height; y+=2) {
		for (unsigned int x = 0; x < width; x+=16) {
			const size_t resultOffset = (y * width + x) * 3;

			// Get both y vectors
			std::memcpy(&y1[0], ycbcr.y.data() + y * width + x, sizeof (__m128i));
			std::memcpy(&y1[1], ycbcr.y.data() + (y + 1) * width + x, sizeof (__m128i));

			// Get cb and cr vector
			cb = _mm_loadl_epi64(reinterpret_cast<const __m128i *>(ycbcr.cb.data() + (y / 2) * (width / 2) + x / 2));
			cr = _mm_loadl_epi64(reinterpret_cast<const __m128i *>(ycbcr.cr.data() + (y / 2) * (width / 2) + x / 2));

			// Unpack and shift the y values to match the other integer factors
			y2[0] = _mm_slli_epi16(_mm_unpacklo_epi8(y1[0], zero), 6);
			y2[1] = _mm_slli_epi16(_mm_unpackhi_epi8(y1[0], zero), 6);
			y2[2] = _mm_slli_epi16(_mm_unpacklo_epi8(y1[1], zero), 6);
			y2[3] = _mm_slli_epi16(_mm_unpackhi_epi8(y1[1], zero), 6);

			// Get uv difference and unpack it to 16 bit
			cb = _mm_unpacklo_epi8(cb, zero);
			cb0 = _mm_sub_epi16(_mm_unpacklo_epi16(cb, cb), uvdiff);
			cb1 = _mm_sub_epi16(_mm_unpackhi_epi16(cb, cb), uvdiff);
			cr = _mm_unpacklo_epi8(cr, zero);
			cr0 = _mm_sub_epi16(_mm_unpacklo_epi16(cr, cr), uvdiff);
			cr1 = _mm_sub_epi16(_mm_unpackhi_epi16(cr, cr), uvdiff);

			// Calculate factors for later calcualtion
			r_cr[0] = _mm_mullo_epi16(fac_r_cr, cr0);
			r_cr[1] = _mm_mullo_epi16(fac_r_cr, cr1);
			g_cb[0] = _mm_mullo_epi16(fac_g_cb, cb0);
			g_cb[1] = _mm_mullo_epi16(fac_g_cb, cb1);
			g_cr[0] = _mm_mullo_epi16(fac_g_cr, cr0);
			g_cr[1] = _mm_mullo_epi16(fac_g_cr, cr1);
			b_cb[0] = _mm_mullo_epi16(fac_b_cb, cb0);
			b_cb[1] = _mm_mullo_epi16(fac_b_cb, cb1);

			// Convert two rows of values after each other
			for (int i = 0; i < 2; ++i) {
				// Convert two ycbcr vectors two rgb
				for (int j = 0; j < 2; ++j) {
					r[j] = _mm_srai_epi16(_mm_add_epi16(y2[i * 2 + j], r_cr[j]), 6);
					g[j] = _mm_srai_epi16(_mm_sub_epi16(_mm_sub_epi16(y2[i * 2 + j], g_cr[j]), g_cb[j]), 6);
					b[j] = _mm_srai_epi16(_mm_add_epi16(y2[i * 2 + j], b_cb[j]), 6);
				}

				// Pack rgb together to 8 bit
				r[0] = _mm_packus_epi16(r[0], r[1]);
				g[0] = _mm_packus_epi16(g[0], g[1]);
				b[0] = _mm_packus_epi16(b[0], b[1]);

				// Copy the rgb data to arrays for better access
				std::memcpy(result_r, &r[0], sizeof(__m128i));
				std::memcpy(result_g, &g[0], sizeof(__m128i));
				std::memcpy(result_b, &b[0], sizeof(__m128i));

				// Write the result data
				for (int j = 0; j < 16; ++j) {
					rgb[resultOffset + j * 3 + 0 + i * width * 3] = result_r[j];
					rgb[resultOffset + j * 3 + 1 + i * width * 3] = result_g[j];
					rgb[resultOffset + j * 3 + 2 + i * width * 3] = result_b[j];
				}
			}
		}
	}
#else
	throw CreateException("OpenAWE was not compiled with SSE2 support");
#endif
}

} // End of namespace Codecs
