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

#ifndef OPENAWE_YUV2RGB_H
#define OPENAWE_YUV2RGB_H

#include "src/common/types.h"

#include "src/codecs/videostream.h"

namespace Codecs {

/*!
 * Convert the data from an ycbcr buffer to the rgb space of an array. This array is assumed to have the necessary
 * size.
 * @param ycbcr The input ycbcr buffer to convert
 * @param rgb The output rgb buffer to write the results into
 * @param width The width of the video frame
 * @param height The height of the video frame
 */
void convertYUV2RGB(const YCbCrBuffer &ycbcr, byte *rgb, unsigned int width, unsigned int height);

} // End of namespace Codecs

#endif //OPENAWE_YUV2RGB_H
