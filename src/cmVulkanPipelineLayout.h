/**
	\brief Vulkan流水线布局封装类
*/
//------------------------------------------------------------------------------
#ifndef _CM_VULKAN_PIPELINE_LAYOUT_h_
#define _CM_VULKAN_PIPELINE_LAYOUT_h_ "cmVulkanPipelineLayout.h"
//------------------------------------------------------------------------------
#include "cmPublic.h"
#include "cmGLFWVulkan.h"
#include <vector>
//------------------------------------------------------------------------------
CM_BEGIN
//------------------------------------------------------------------------------
class VulkanDevice;
//------------------------------------------------------------------------------
class VulkanPipelineLayout
{
public:
	friend class VulkanDevice;
	friend class VulkanCommandBuffer;

	VulkanPipelineLayout();
	VulkanPipelineLayout(VulkanPipelineLayout& r);
	~VulkanPipelineLayout();

	VulkanPipelineLayout& operator=(const VulkanPipelineLayout& r);
	VulkanPipelineLayout& operator=(VulkanPipelineLayout&& r);

	bool operator!() const
	{
		return VK_NULL_HANDLE == f_PipelineLayout;
	}
private:
	VulkanPipelineLayout(VkPipelineLayout pipelineLayout, VulkanDevice* device);

	VulkanDevice* f_Device;
	VkPipelineLayout f_PipelineLayout;

	int* f_Usage;
};
//------------------------------------------------------------------------------
CM_END
//------------------------------------------------------------------------------
#endif//_CM_VULKAN_PIPELINE_LAYOUT_h_