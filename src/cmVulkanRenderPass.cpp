/**
	\brief Vulkan渲染通道封装类
*/
//------------------------------------------------------------------------------
#include "cmVulkanDevice.h"
#include "cmVulkanRenderPass.h"
//------------------------------------------------------------------------------
CM_BEGIN
//------------------------------------------------------------------------------
VulkanRenderPass::VulkanRenderPass()
: f_RenderPass(VK_NULL_HANDLE), f_Usage(new int(1)), f_Device(nullptr)
{

}
//------------------------------------------------------------------------------
VulkanRenderPass::VulkanRenderPass(VkRenderPass renderPass, VulkanDevice* device)
	: f_RenderPass(renderPass), f_Usage(new int(1)), f_Device(device)
{
}
//------------------------------------------------------------------------------
VulkanRenderPass::VulkanRenderPass(VulkanRenderPass& r)
	: f_RenderPass(r.f_RenderPass), f_Usage(r.f_Usage), f_Device(r.f_Device)
{
	++(*f_Usage);
}
//------------------------------------------------------------------------------
VulkanRenderPass::~VulkanRenderPass()
{
	if (0 == --(*f_Usage))
	{
		if (VK_NULL_HANDLE != f_RenderPass)
		{
			f_Device->f_DeviceFunctions.vkDestroyRenderPass(f_Device->f_Device, f_RenderPass, nullptr);
		}
		delete f_Usage;
	}
}
//------------------------------------------------------------------------------
VulkanRenderPass& VulkanRenderPass::operator=(const VulkanRenderPass& r)
{
	if (this != &r)
	{
		if (0 == --(*f_Usage))
		{
			if (VK_NULL_HANDLE != f_RenderPass)
			{
				f_Device->f_DeviceFunctions.vkDestroyRenderPass(f_Device->f_Device, f_RenderPass, nullptr);
			}
			delete f_Usage;
		}
		f_RenderPass = r.f_RenderPass;
		f_Device = r.f_Device;
		f_Usage = r.f_Usage;
		++(*f_Usage);
	}
	return *this;
}
//------------------------------------------------------------------------------
VulkanRenderPass& VulkanRenderPass::operator=(VulkanRenderPass&& r)
{
	if (this != &r)
	{
		std::swap(f_RenderPass, r.f_RenderPass);
		std::swap(f_Device, r.f_Device);
		std::swap(f_Usage, r.f_Usage);
	}
	return *this;
}
//------------------------------------------------------------------------------
CM_END
