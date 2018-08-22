/**
	\brief Vulkan instance·â×°Àà
*/
//------------------------------------------------------------------------------
#include "cmVulkanInstance.h"
#include "cmVulkanSurface.h"
//------------------------------------------------------------------------------
CM_BEGIN
//------------------------------------------------------------------------------
VulkanSurface::VulkanSurface()
: f_Surface(VK_NULL_HANDLE), f_Usage(new int(1))
{

}
//------------------------------------------------------------------------------
VulkanSurface::VulkanSurface(VulkanSurface& r)
: f_Instance(r.f_Instance), f_Surface(r.f_Surface), f_Usage(r.f_Usage)
{
	++(*f_Usage);
}
//------------------------------------------------------------------------------
VulkanSurface::VulkanSurface(VkInstance instance, VkSurfaceKHR surface)
	: f_Instance(instance), f_Surface(surface), f_Usage(new int(1))
{

}
//------------------------------------------------------------------------------
VulkanSurface::~VulkanSurface()
{
	if (0 == --(*f_Usage))
	{
		if (VK_NULL_HANDLE != f_Surface)
		{
			vkDestroySurfaceKHR(f_Instance, f_Surface, nullptr);
		}
		delete f_Usage;
	}
}
//------------------------------------------------------------------------------
VulkanSurface& VulkanSurface::operator=(const VulkanSurface& r)
{
	if (this != &r)
	{
		if (0 == --(*f_Usage))
		{
			if (VK_NULL_HANDLE != f_Surface)
			{
				vkDestroySurfaceKHR(f_Instance, f_Surface, nullptr);
			}
			delete f_Usage;
		}
		f_Instance = r.f_Instance;
		f_Surface = r.f_Surface;
		f_Usage = r.f_Usage;
		++(*f_Usage);
	}
	return *this;
}
//------------------------------------------------------------------------------
VulkanSurface& VulkanSurface::operator=(VulkanSurface&& r)
{
	if (this != &r)
	{
		std::swap(f_Instance, r.f_Instance);
		std::swap(f_Surface, r.f_Surface);
		std::swap(f_Usage, r.f_Usage);
	}
	return *this;
}
//------------------------------------------------------------------------------
CM_END
