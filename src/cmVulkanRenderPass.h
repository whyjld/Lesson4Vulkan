/**
	\brief Vulkan渲染通道封装类
*/
//------------------------------------------------------------------------------
#ifndef _CM_VULKAN_RENDERPASS_h_
#define _CM_VULKAN_RENDERPASS_h_ "cmVulkanRenderPass.h"
//------------------------------------------------------------------------------
#include "cmPublic.h"
#include "cmGLFWVulkan.h"
//------------------------------------------------------------------------------
CM_BEGIN
//------------------------------------------------------------------------------
class VulkanRenderPass
{
public:
	friend class VulkanDevice;
	friend class VulkanCommandBuffer;

	VulkanRenderPass();
	VulkanRenderPass(VulkanRenderPass& r);
	~VulkanRenderPass();

	VulkanRenderPass& operator=(const VulkanRenderPass& r);
	VulkanRenderPass& operator=(VulkanRenderPass&& r);

	bool operator!() const
	{
		return VK_NULL_HANDLE == f_RenderPass;
	}
private:
	VulkanRenderPass(VkRenderPass renderPass, VulkanDevice* device);

	VulkanDevice* f_Device;
	VkRenderPass f_RenderPass;

	int* f_Usage;
};
//------------------------------------------------------------------------------
CM_END
//------------------------------------------------------------------------------
#endif//_CM_VULKAN_RENDERPASS_h_