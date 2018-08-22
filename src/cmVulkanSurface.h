/**
	\brief Vulkan±Ì√Ê¿‡
*/
//------------------------------------------------------------------------------
#ifndef _CM_VULKAN_SURFACE_h_
#define _CM_VULKAN_SURFACE_h_ "cmVulkanSurface.h"
//------------------------------------------------------------------------------
#include "cmPublic.h"
#include "cmGLFWVulkan.h"
#include <vector>
//------------------------------------------------------------------------------
#if defined(VK_KHR_surface)
//------------------------------------------------------------------------------
CM_BEGIN
//------------------------------------------------------------------------------
class VulkanSurface
{
public:
	friend class VulkanInstance;
	friend class VulkanPhysicalDevice;
	friend class VulkanDevice;

	VulkanSurface();
	VulkanSurface(VulkanSurface& r);
	~VulkanSurface();

	VulkanSurface& operator=(const VulkanSurface& r);
	VulkanSurface& operator=(VulkanSurface&& r);

	bool operator!() const
	{
		return VK_NULL_HANDLE == f_Surface;
	}
private:
	VulkanSurface(VkInstance instance, VkSurfaceKHR surface);

	VkInstance f_Instance;
	VkSurfaceKHR f_Surface;

	int* f_Usage;
};
//------------------------------------------------------------------------------
CM_END
//------------------------------------------------------------------------------
#endif//VK+KHR_surface
//------------------------------------------------------------------------------
#endif//_CM_VULKAN_SURFACE_h_