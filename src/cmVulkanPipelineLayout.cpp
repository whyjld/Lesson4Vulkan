/**
	\brief Vulkan 流水线布局封装类
*/
//------------------------------------------------------------------------------
#include "cmVulkanDevice.h"
#include "cmVulkanPipelineLayout.h"
//------------------------------------------------------------------------------
CM_BEGIN
//------------------------------------------------------------------------------
VulkanPipelineLayout::VulkanPipelineLayout()
	: f_Device(nullptr), f_PipelineLayout(VK_NULL_HANDLE), f_Usage(new int(1))
{

}
//------------------------------------------------------------------------------
VulkanPipelineLayout::VulkanPipelineLayout(VkPipelineLayout pipelineLayout, VulkanDevice* device)
	: f_Device(device), f_PipelineLayout(pipelineLayout), f_Usage(new int(1))
{
}
//------------------------------------------------------------------------------
VulkanPipelineLayout::VulkanPipelineLayout(VulkanPipelineLayout& r)
	: f_Device(r.f_Device), f_PipelineLayout(r.f_PipelineLayout), f_Usage(r.f_Usage)
{
	++(*f_Usage);
}
//------------------------------------------------------------------------------
VulkanPipelineLayout::~VulkanPipelineLayout()
{
	if (0 == --(*f_Usage))
	{
		if (VK_NULL_HANDLE != f_PipelineLayout)
		{
			f_Device->f_DeviceFunctions.vkDestroyPipelineLayout(f_Device->f_Device, f_PipelineLayout, nullptr);
		}
		delete f_Usage;
	}
}
//------------------------------------------------------------------------------
VulkanPipelineLayout& VulkanPipelineLayout::operator=(const VulkanPipelineLayout& r)
{
	if (this != &r)
	{
		if (0 == --(*f_Usage))
		{
			if (VK_NULL_HANDLE != f_PipelineLayout)
			{
				f_Device->f_DeviceFunctions.vkDestroyPipelineLayout(f_Device->f_Device, f_PipelineLayout, nullptr);
			}
			delete f_Usage;
		}
		f_Device = r.f_Device;
		f_PipelineLayout = r.f_PipelineLayout;
		f_Usage = r.f_Usage;
		++(*f_Usage);
	}
	return *this;
}
//------------------------------------------------------------------------------
VulkanPipelineLayout& VulkanPipelineLayout::operator=(VulkanPipelineLayout&& r)
{
	if (this != &r)
	{
		std::swap(f_Device, r.f_Device);
		std::swap(f_PipelineLayout, r.f_PipelineLayout);
		std::swap(f_Usage, r.f_Usage);
	}
	return *this;
}
//------------------------------------------------------------------------------
CM_END
