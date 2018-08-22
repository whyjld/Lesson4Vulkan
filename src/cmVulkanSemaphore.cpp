/**
	\brief Vulkan ÐÅºÅ·â×°Àà
*/
//------------------------------------------------------------------------------
#include "cmVulkanDevice.h"
#include "cmVulkanSemaphore.h"
//------------------------------------------------------------------------------
CM_BEGIN
//------------------------------------------------------------------------------
VulkanSemaphore::VulkanSemaphore()
	: f_Device(nullptr), f_Semaphore(VK_NULL_HANDLE), f_Usage(new int(1))
{

}
//------------------------------------------------------------------------------
VulkanSemaphore::VulkanSemaphore(VkSemaphore semaphore, VulkanDevice* device)
	: f_Device(device), f_Semaphore(semaphore), f_Usage(new int(1))
{
}
//------------------------------------------------------------------------------
VulkanSemaphore::VulkanSemaphore(VulkanSemaphore& r)
	: f_Device(r.f_Device), f_Semaphore(r.f_Semaphore), f_Usage(r.f_Usage)
{
	++(*f_Usage);
}
//------------------------------------------------------------------------------
VulkanSemaphore::~VulkanSemaphore()
{
	if (0 == --(*f_Usage))
	{
		if (VK_NULL_HANDLE != f_Semaphore)
		{
			f_Device->f_DeviceFunctions.vkDestroySemaphore(f_Device->f_Device, f_Semaphore, nullptr);
		}
		delete f_Usage;
	}
}
//------------------------------------------------------------------------------
VulkanSemaphore& VulkanSemaphore::operator=(const VulkanSemaphore& r)
{
	if (this != &r)
	{
		if (0 == --(*f_Usage))
		{
			if (VK_NULL_HANDLE != f_Semaphore)
			{
				f_Device->f_DeviceFunctions.vkDestroySemaphore(f_Device->f_Device, f_Semaphore, nullptr);
			}
			delete f_Usage;
		}
		f_Device = r.f_Device;
		f_Semaphore = r.f_Semaphore;
		f_Usage = r.f_Usage;
		++(*f_Usage);
	}
	return *this;
}
//------------------------------------------------------------------------------
VulkanSemaphore& VulkanSemaphore::operator=(VulkanSemaphore&& r)
{
	if (this != &r)
	{
		std::swap(f_Device, r.f_Device);
		std::swap(f_Semaphore, r.f_Semaphore);
		std::swap(f_Usage, r.f_Usage);
	}
	return *this;
}
//------------------------------------------------------------------------------
CM_END
