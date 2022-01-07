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

#ifndef AWE_VULKANCONTEXT_H
#define AWE_VULKANCONTEXT_H

#ifdef WITH_VULKAN
#include <vulkan/vulkan.h>
#endif // WITH_VULKAN

#include "context.h"

namespace Platform {

class VulkanContext : public Context {
public:
#ifdef WITH_VULKAN
	virtual const char **getInstanceExtensions(unsigned int &numExtensions) = 0;
	virtual void createWindowSurface(VkInstance &instance, VkSurfaceKHR &surface) = 0;
#endif //WITH_VULKAN
};

}

#endif //AWE_VULKANCONTEXT_H
