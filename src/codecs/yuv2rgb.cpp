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
#include "src/codecs/yuv2rgb.h"

namespace Codecs {

void convertYUV2RGB(const YCbCrBuffer &ycbcr, byte *rgb, unsigned int width, unsigned int height) {
	for (unsigned int y = 0; y < height; ++y) {
		for (unsigned int x = 0; x < width; ++x) {
			const float y1 = ycbcr.y[y * width + x];
			const float cb = ycbcr.cb[(y / 2) * (width / 2) + x / 2];
			const float cr = ycbcr.cr[(y / 2) * (width / 2) + x / 2];

			const short r = y1 + 1.4075 * (cr - 128);
			const short g = y1 - 0.3455 * (cb - 128) - 0.7169 * (cr - 128);
			const short b = y1 + 1.7790 * (cb - 128);

			rgb[(y * width + x) * 3]     = std::clamp<byte>(r, 0, 255);
			rgb[(y * width + x) * 3 + 1] = std::clamp<byte>(g, 0, 255);
			rgb[(y * width + x) * 3 + 2] = std::clamp<byte>(b, 0, 255);
		}
	}
}

} // End of namespace Codecs
