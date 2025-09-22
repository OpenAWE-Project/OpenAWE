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

#ifndef GRAPHICS_IMAGES_SH_H
#define GRAPHICS_IMAGES_SH_H

#include <memory>

#include "src/graphics/images/surface.h"

namespace Graphics {

/**
 * Generate the spherical harmonics basis map to the specified order with the image size.
 *
 * \param order To which order the spherical harmonics should be generated
 * \param imageSize The size of the image in pixels per order and degree
 * \return The generated spherical harmonics map as shared surface
 */
SurfacePtr generateSHMap(unsigned int order, size_t imageSize);

} // End of namespace Graphics

#endif // GRAPHICS_IMAGES_SH_H
