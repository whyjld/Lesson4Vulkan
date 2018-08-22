/**
	\brief Vulkan ÐÅºÅ·â×°Àà
*/
//------------------------------------------------------------------------------
#include "cmVulkanDevice.h"
#include "cmVulkanFence.h"
//------------------------------------------------------------------------------
CM_BEGIN
//------------------------------------------------------------------------------
VulkanFence::VulkanFence()
	: f_Device(nullptr), f_Fence(VK_NULL_HANDLE), f_Usage(new int(1))
{

}
//------------------------------------------------------------------------------
VulkanFence::VulkanFence(VkFence fence, VulkanDevice* device)
	: f_Device(device), f_Fence(fence), f_Usage(new int(1))
{
}
//------------------------------------------------------------------------------
VulkanFence::VulkanFence(const VulkanFence& r)
	: f_Device(r.f_Device), f_Fence(r.f_Fence), f_Usage(r.f_Usage)
{
	++(*f_Usage);
}
//------------------------------------------------------------------------------
VulkanFence::~VulkanFence()
{
	if (0 == --(*f_Usage))
	{
		if (VK_NULL_HANDLE != f_Fence)
		{
			f_Device->f_DeviceFunctions.vkDestroyFence(f_Device->f_Device, f_Fence, nullptr);
		}
		delete f_Usage;
	}
}
//------------------------------------------------------------------------------
VulkanFence& VulkanFence::operator=(const VulkanFence& r)
{
	if (this != &r)
	{
		if (0 == --(*f_Usage))
		{
			if (VK_NULL_HANDLE != f_Fence)
			{
				f_Device->f_DeviceFunctions.vkDestroyFence(f_Device->f_Device, f_Fence, nullptr);
			}
			delete f_Usage;
		}
		f_Device = r.f_Device;
		f_Fence = r.f_Fence;
		f_Usage = r.f_Usage;
		++(*f_Usage);
	}
	return *this;
}
//------------------------------------------------------------------------------
VulkanFence& VulkanFence::operator=(VulkanFence&& r)
{
	if (this != &r)
	{
		std::swap(f_Device, r.f_Device);
		std::swap(f_Fence, r.f_Fence);
		std::swap(f_Usage, r.f_Usage);
	}
	return *this;
}
//------------------------------------------------------------------------------
CM_END
