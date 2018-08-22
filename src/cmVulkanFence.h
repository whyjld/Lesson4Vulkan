/**
	\brief VulkanÕ¤À¸·â×°Àà
*/
//------------------------------------------------------------------------------
#ifndef _CM_VULKAN_FENCE_h_
#define _CM_VULKAN_FENCE_h_ "cmVulkanFence.h"
//------------------------------------------------------------------------------
#include "cmPublic.h"
#include "cmGLFWVulkan.h"
#include <vector>
//------------------------------------------------------------------------------
CM_BEGIN
//------------------------------------------------------------------------------
class VulkanDevice;
//------------------------------------------------------------------------------
class VulkanFence
{
public:
	friend class VulkanDevice;
	friend class VulkanSwapchain;
	friend class VulkanQueue;

	VulkanFence();
	VulkanFence(const VulkanFence& r);
	~VulkanFence();

	VulkanFence& operator=(const VulkanFence& r);
	VulkanFence& operator=(VulkanFence&& r);

	bool operator!() const
	{
		return VK_NULL_HANDLE == f_Fence;
	}
private:
	VulkanFence(VkFence fence, VulkanDevice* device);

	VulkanDevice* f_Device;
	VkSemaphore f_Fence;

	int* f_Usage;
};
//------------------------------------------------------------------------------
CM_END
//------------------------------------------------------------------------------
#endif//_CM_VULKAN_FENCE_h_