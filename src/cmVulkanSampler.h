/**
	\brief Vulkan采样器封装类
*/
//------------------------------------------------------------------------------
#ifndef _CM_VULKAN_SAMPLER_h_
#define _CM_VULKAN_SAMPLER_h_ "cmVulkanSampler.h"
//------------------------------------------------------------------------------
#include "cmPublic.h"
#include "cmGLFWVulkan.h"
#include <vector>
//------------------------------------------------------------------------------
CM_BEGIN
//------------------------------------------------------------------------------
class VulkanDevice;
//------------------------------------------------------------------------------
class VulkanSampler
{
public:
	friend class VulkanDevice;
	friend class VulkanDescriptorSet;

	VulkanSampler();
	VulkanSampler(VulkanSampler& r);
	~VulkanSampler();

	VulkanSampler& operator=(const VulkanSampler& r);
	VulkanSampler& operator=(VulkanSampler&& r);

	bool operator!() const
	{
		return VK_NULL_HANDLE == f_Sampler;
	}
private:
	VulkanSampler(VkSampler sampler, VulkanDevice* device);

	VulkanDevice* f_Device;
	VkSampler f_Sampler;

	int* f_Usage;
};
//------------------------------------------------------------------------------
CM_END
//------------------------------------------------------------------------------
#endif//_CM_VULKAN_SAMPLER_h_