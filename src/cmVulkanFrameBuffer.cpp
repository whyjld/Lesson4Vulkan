/**
	\brief Vulkan ÐÅºÅ·â×°Àà
*/
//------------------------------------------------------------------------------
#include "cmVulkanDevice.h"
#include "cmVulkanFramebuffer.h"
//------------------------------------------------------------------------------
CM_BEGIN
//------------------------------------------------------------------------------
VulkanFramebuffer::VulkanFramebuffer()
	: f_Device(nullptr), f_Framebuffer(VK_NULL_HANDLE), f_Usage(new int(1))
{

}
//------------------------------------------------------------------------------
VulkanFramebuffer::VulkanFramebuffer(VkFramebuffer framebuffer, VulkanDevice* device)
	: f_Device(device), f_Framebuffer(framebuffer), f_Usage(new int(1))
{
}
//------------------------------------------------------------------------------
VulkanFramebuffer::VulkanFramebuffer(VulkanFramebuffer& r)
	: f_Device(r.f_Device), f_Framebuffer(r.f_Framebuffer), f_Usage(r.f_Usage)
{
	++(*f_Usage);
}
//------------------------------------------------------------------------------
VulkanFramebuffer::~VulkanFramebuffer()
{
	if (0 == --(*f_Usage))
	{
		if (VK_NULL_HANDLE != f_Framebuffer)
		{
			f_Device->f_DeviceFunctions.vkDestroyFramebuffer(f_Device->f_Device, f_Framebuffer, nullptr);
		}
		delete f_Usage;
	}
}
//------------------------------------------------------------------------------
VulkanFramebuffer& VulkanFramebuffer::operator=(const VulkanFramebuffer& r)
{
	if (this != &r)
	{
		if (0 == --(*f_Usage))
		{
			if (VK_NULL_HANDLE != f_Framebuffer)
			{
				f_Device->f_DeviceFunctions.vkDestroyFramebuffer(f_Device->f_Device, f_Framebuffer, nullptr);
			}
			delete f_Usage;
		}
		f_Device = r.f_Device;
		f_Framebuffer = r.f_Framebuffer;
		f_Usage = r.f_Usage;
		++(*f_Usage);
	}
	return *this;
}
//------------------------------------------------------------------------------
VulkanFramebuffer& VulkanFramebuffer::operator=(VulkanFramebuffer&& r)
{
	if (this != &r)
	{
		std::swap(f_Device, r.f_Device);
		std::swap(f_Framebuffer, r.f_Framebuffer);
		std::swap(f_Usage, r.f_Usage);
	}
	return *this;
}
//------------------------------------------------------------------------------
CM_END
