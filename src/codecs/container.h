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

#ifndef OPENAWE_CONTAINER_H
#define OPENAWE_CONTAINER_H

#include <cstddef>
#include "src/codecs/audiostream.h"
#include "src/codecs/videostream.h"

namespace Codecs {

/*!
 * \brief Base class for handling media containers
 *
 * Base class for handling media containers which can contain multiple audio streams and an optional video stream.
 */
class Container {
public:
	/*!
	 * Get the number of audio streams contained in this container file
	 * \return The number of audio stream contained in this container file
	 */
	virtual size_t getNumAudioStreams() const = 0;

	/*!
	 * Get if the video stream has a video stream contained in this container
	 * \return If this container has a video stream
	 */
	virtual bool hasVideoStream() const = 0;

	/*!
	 * Create the audio stream from this container for the specified index
	 * \param index The index of the audio stream to create
	 * \return The new created audio stream
	 */
	virtual AudioStream *createAudioStream(size_t index) = 0;

	/*!
	 * Create the video stream from this stream
	 * \return The newly created video stream
	 */
	virtual VideoStream *createVideoStream() = 0;
};

} // End of namespace Codecs

#endif //OPENAWE_CONTAINER_H
