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

#if __SSSE3__
#	include <tmmintrin.h>
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

void convertYUV2RGB_SSSE3(const YCbCrBuffer &ycbcr, byte *rgb, unsigned int width, unsigned int height) {
#if __SSSE3__
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

    __m128i rmask1 = _mm_set_epi8(
            5, 128, 128, 4, 128, 128, 3, 128,
            128, 2, 128, 128, 1, 128, 128, 0
    );
    __m128i rmask2 = _mm_set_epi8(
            128, 10, 128, 128, 9, 128, 128, 8,
            128, 128, 7, 128, 128, 6, 128, 128
    );
    __m128i rmask3 = _mm_set_epi8(
            128, 128, 15, 128, 128, 14, 128, 128,
            13, 128, 128, 12, 128, 128, 11, 128
    );
    __m128i gmask1 = _mm_set_epi8(
            128, 128, 4, 128, 128, 3, 128, 128,
            2, 128, 128, 1, 128, 128, 0, 128
    );
    __m128i gmask2 = _mm_set_epi8(
            10, 128, 128, 9, 128, 128, 8, 128,
            128, 7, 128, 128, 6, 128, 128, 5
    );
    __m128i gmask3 = _mm_set_epi8(
            128, 15, 128, 128, 14, 128, 128, 13,
            128, 128, 12, 128, 128, 11, 128, 128
    );
    __m128i bmask1 = _mm_set_epi8(
            128, 4, 128, 128, 3, 128, 128,
            2, 128, 128, 1, 128, 128, 0, 128, 128
    );
    __m128i bmask2 = _mm_set_epi8(
            128, 128, 9, 128, 128, 8, 128,
            128, 7, 128, 128, 6, 128, 128, 5, 128
    );
    __m128i bmask3 = _mm_set_epi8(
            15, 128, 128, 14, 128, 128, 13, 128,
            128, 12, 128, 128, 11, 128, 128, 10
    );

    __m128i shuffledResult[3];
    __m128i shuffledResultR[6], shuffledResultG[6], shuffledResultB[6];


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

				shuffledResultR[0] = _mm_shuffle_epi8(r[0], rmask1);
				shuffledResultR[1] = _mm_shuffle_epi8(r[0], rmask2);
				shuffledResultR[2] = _mm_shuffle_epi8(r[0], rmask3);
				shuffledResultG[0] = _mm_shuffle_epi8(g[0], gmask1);
				shuffledResultG[1] = _mm_shuffle_epi8(g[0], gmask2);
				shuffledResultG[2] = _mm_shuffle_epi8(g[0], gmask3);
				shuffledResultB[0] = _mm_shuffle_epi8(b[0], bmask1);
				shuffledResultB[1] = _mm_shuffle_epi8(b[0], bmask2);
				shuffledResultB[2] = _mm_shuffle_epi8(b[0], bmask3);

				shuffledResult[0] = _mm_or_si128(shuffledResultR[0], _mm_or_si128(shuffledResultB[0], shuffledResultG[0]));
				shuffledResult[1] = _mm_or_si128(shuffledResultR[1], _mm_or_si128(shuffledResultB[1], shuffledResultG[1]));
				shuffledResult[2] = _mm_or_si128(shuffledResultR[2], _mm_or_si128(shuffledResultB[2], shuffledResultG[2]));

				std::memcpy(reinterpret_cast<void *>(rgb + resultOffset + i * width * 3), shuffledResult, sizeof(shuffledResult));
			}
		}
	}
#else
	throw CreateException("OpenAWE was not compiled with SSE2 support");
#endif
}

} // End of namespace Codecs
