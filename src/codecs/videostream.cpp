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

#include "videostream.h"

namespace Codecs {

VideoStream::VideoStream(unsigned int width, unsigned int height, float fps) :
	_width(width),
	_height(height),
	_fps(fps) {
}

float VideoStream::getFps() const {
	return _fps;
}

unsigned int VideoStream::getWidth() const {
	return _width;
}

unsigned int VideoStream::getHeight() const {
	return _height;
}

} // Codecs