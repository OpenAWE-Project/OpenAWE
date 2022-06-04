/**
 * MojoShader; generate shader programs from bytecode of compiled
 *  Direct3D shaders.
 *
 * Please see the file LICENSE.txt in the source's root directory.
 *
 *  This file written by Ryan C. Gordon.
 */

/*
 * vkInstance, created by global vkCreateInstance function
 */

#ifndef VULKAN_INSTANCE_FUNCTION
#error VULKAN_INSTANCE_FUNCTION macro was undefined!
#endif
VULKAN_INSTANCE_FUNCTION(vkGetPhysicalDeviceMemoryProperties)

/*
 * vkDevice, created by a vkInstance
 */

#ifndef VULKAN_DEVICE_FUNCTION
#error VULKAN_DEVICE_FUNCTION macro was undefined!
#endif
VULKAN_DEVICE_FUNCTION(vkAllocateMemory)
VULKAN_DEVICE_FUNCTION(vkBindBufferMemory)
VULKAN_DEVICE_FUNCTION(vkCreateBuffer)
VULKAN_DEVICE_FUNCTION(vkCreateShaderModule)
VULKAN_DEVICE_FUNCTION(vkDestroyBuffer)
VULKAN_DEVICE_FUNCTION(vkDestroyShaderModule)
VULKAN_DEVICE_FUNCTION(vkFreeMemory)
VULKAN_DEVICE_FUNCTION(vkGetBufferMemoryRequirements)
VULKAN_DEVICE_FUNCTION(vkMapMemory)
VULKAN_DEVICE_FUNCTION(vkUnmapMemory)

/*
 * Redefine these every time you include this header!
 */
#undef VULKAN_INSTANCE_FUNCTION
#undef VULKAN_DEVICE_FUNCTION
