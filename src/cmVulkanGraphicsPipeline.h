/**
	\brief Vulkan图形流水线封装类
*/
//------------------------------------------------------------------------------
#ifndef _CM_VULKAN_GRAPHICS_PIPELINE_h_
#define _CM_VULKAN_GRAPHICS_PIPELINE_h_ "cmVulkanGraphicsPipeline.h"
//------------------------------------------------------------------------------
#include "cmPublic.h"
#include "cmGLFWVulkan.h"
#include <vector>
//------------------------------------------------------------------------------
CM_BEGIN
//------------------------------------------------------------------------------
class VulkanDevice;
//------------------------------------------------------------------------------
class VulkanGraphicsPipeline
{
public:
	friend class VulkanDevice;
	friend class VulkanCommandBuffer;

	VulkanGraphicsPipeline();
	VulkanGraphicsPipeline(VulkanGraphicsPipeline& r);
	~VulkanGraphicsPipeline();

	VulkanGraphicsPipeline& operator=(const VulkanGraphicsPipeline& r);
	VulkanGraphicsPipeline& operator=(VulkanGraphicsPipeline&& r);

	bool operator!() const
	{
		return VK_NULL_HANDLE == f_Pipeline;
	}
private:
	VulkanGraphicsPipeline(VkPipeline pipeline, VulkanDevice* device);

	VulkanDevice* f_Device;
	VkPipeline f_Pipeline;

	int* f_Usage;
};
//------------------------------------------------------------------------------
CM_END
//------------------------------------------------------------------------------
#endif//_CM_VULKAN_GRAPHICS_PIPELINE_h_