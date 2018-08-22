/**
	\brief Vulkan 图形流水线封装类
*/
//------------------------------------------------------------------------------
#include "cmVulkanDevice.h"
#include "cmVulkanGraphicsPipeline.h"
//------------------------------------------------------------------------------
CM_BEGIN
//------------------------------------------------------------------------------
VulkanGraphicsPipeline::VulkanGraphicsPipeline()
: f_Device(nullptr), f_Pipeline(VK_NULL_HANDLE), f_Usage(new int(1))
{

}
//------------------------------------------------------------------------------
VulkanGraphicsPipeline::VulkanGraphicsPipeline(VkSemaphore semaphore, VulkanDevice* device)
	: f_Device(device), f_Pipeline(semaphore), f_Usage(new int(1))
{
}
//------------------------------------------------------------------------------
VulkanGraphicsPipeline::VulkanGraphicsPipeline(VulkanGraphicsPipeline& r)
	: f_Device(r.f_Device), f_Pipeline(r.f_Pipeline), f_Usage(r.f_Usage)
{
	++(*f_Usage);
}
//------------------------------------------------------------------------------
VulkanGraphicsPipeline::~VulkanGraphicsPipeline()
{
	if (0 == --(*f_Usage))
	{
		if (VK_NULL_HANDLE != f_Pipeline)
		{
			f_Device->f_DeviceFunctions.vkDestroyPipeline(f_Device->f_Device, f_Pipeline, nullptr);
		}
		delete f_Usage;
	}
}
//------------------------------------------------------------------------------
VulkanGraphicsPipeline& VulkanGraphicsPipeline::operator=(const VulkanGraphicsPipeline& r)
{
	if (this != &r)
	{
		if (0 == --(*f_Usage))
		{
			if (VK_NULL_HANDLE != f_Pipeline)
			{
				f_Device->f_DeviceFunctions.vkDestroyPipeline(f_Device->f_Device, f_Pipeline, nullptr);
			}
			delete f_Usage;
		}
		f_Device = r.f_Device;
		f_Pipeline = r.f_Pipeline;
		f_Usage = r.f_Usage;
		++(*f_Usage);
	}
	return *this;
}
//------------------------------------------------------------------------------
VulkanGraphicsPipeline& VulkanGraphicsPipeline::operator=(VulkanGraphicsPipeline&& r)
{
	if (this != &r)
	{
		std::swap(f_Device, r.f_Device);
		std::swap(f_Pipeline, r.f_Pipeline);
		std::swap(f_Usage, r.f_Usage);
	}
	return *this;
}
//------------------------------------------------------------------------------
CM_END
