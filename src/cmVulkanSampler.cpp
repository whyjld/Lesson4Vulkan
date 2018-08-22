/**
	\brief Vulkan采样器封装类
*/
//------------------------------------------------------------------------------
#include "cmVulkanDevice.h"
#include "cmVulkanSampler.h"
//------------------------------------------------------------------------------
CM_BEGIN
//------------------------------------------------------------------------------
VulkanSampler::VulkanSampler()
	: f_Device(nullptr), f_Sampler(VK_NULL_HANDLE), f_Usage(new int(1))
{

}
//------------------------------------------------------------------------------
VulkanSampler::VulkanSampler(VkSampler sampler, VulkanDevice* device)
	: f_Device(device), f_Sampler(sampler), f_Usage(new int(1))
{
}
//------------------------------------------------------------------------------
VulkanSampler::VulkanSampler(VulkanSampler& r)
	: f_Device(r.f_Device), f_Sampler(r.f_Sampler), f_Usage(r.f_Usage)
{
	++(*f_Usage);
}
//------------------------------------------------------------------------------
VulkanSampler::~VulkanSampler()
{
	if (0 == --(*f_Usage))
	{
		if (VK_NULL_HANDLE != f_Sampler)
		{
			f_Device->f_DeviceFunctions.vkDestroySampler(f_Device->f_Device, f_Sampler, nullptr);
		}
		delete f_Usage;
	}
}
//------------------------------------------------------------------------------
VulkanSampler& VulkanSampler::operator=(const VulkanSampler& r)
{
	if (this != &r)
	{
		if (0 == --(*f_Usage))
		{
			if (VK_NULL_HANDLE != f_Sampler)
			{
				f_Device->f_DeviceFunctions.vkDestroySampler(f_Device->f_Device, f_Sampler, nullptr);
			}
			delete f_Usage;
		}
		f_Device = r.f_Device;
		f_Sampler = r.f_Sampler;
		f_Usage = r.f_Usage;
		++(*f_Usage);
	}
	return *this;
}
//------------------------------------------------------------------------------
VulkanSampler& VulkanSampler::operator=(VulkanSampler&& r)
{
	if (this != &r)
	{
		std::swap(f_Device, r.f_Device);
		std::swap(f_Sampler, r.f_Sampler);
		std::swap(f_Usage, r.f_Usage);
	}
	return *this;
}
//------------------------------------------------------------------------------
CM_END
