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

#ifndef OPENAWE_VIDEOSTREAM_H
#define OPENAWE_VIDEOSTREAM_H

#include <vector>

#include "src/common/types.h"

namespace Codecs {

/*!
 * Buffer for storing ycbcr images from a video stream
 */
struct YCbCrBuffer {
	std::vector<byte> y;
	std::vector<byte> cb;
	std::vector<byte> cr;
};

/*!
 * \brief Base class for a video stream
 *
 * This class abstracts a video stream which can read a next frame, check for an end of the stream and give back
 * certain metadata, important for the operation of that video.
 */
class VideoStream {
public:
	/*!
	 * Constructs a new video stream out of the frame size and the frames per second
	 * \param width The width of the video
	 * \param height The height of the video
	 * \param fps frames per second of the video
	 */
	VideoStream(unsigned int width, unsigned int height, float fps);

	/*!
	 * Return the frames per second of this video stream
	 * \return The frames per second of this video stream
	 */
	float getFps() const;

	/*!
	 * Return the width of the video in this video stream
	 * \return The width of the video in this video stream
	 */
	unsigned int getWidth() const;

	/*!
	 * Return the height of the video in this video stream
	 * \return The height of the video in this video stream
	 */
	unsigned int getHeight() const;

	/*!
	 * If this video stream reached its end
	 * \return If this video stream reached its end
	 */
	virtual bool eos() const = 0;

	/*!
	 * Read the next frame as ycbcr buffer
	 * \param ycbcrBuffer The ycbcr buffer to write into
	 */
	virtual void readNextFrame(YCbCrBuffer &ycbcrBuffer) = 0;

private:
	const float _fps;
	const unsigned int _width, _height;
};

} // End of namespace Codecs

#endif //OPENAWE_VIDEOSTREAM_H
