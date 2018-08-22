/**
	\brief VulkanÐÅºÅ·â×°Àà
*/
//------------------------------------------------------------------------------
#ifndef _CM_VULKAN_SEMAPHORE_h_
#define _CM_VULKAN_SEMAPHORE_h_ "cmVulkanSemaphore.h"
//------------------------------------------------------------------------------
#include "cmPublic.h"
#include "cmGLFWVulkan.h"
#include <vector>
//------------------------------------------------------------------------------
CM_BEGIN
//------------------------------------------------------------------------------
class VulkanDevice;
//------------------------------------------------------------------------------
class VulkanSemaphore
{
public:
	friend class VulkanDevice;
	friend class VulkanSwapchain;
	friend class VulkanQueue;

	VulkanSemaphore();
	VulkanSemaphore(VulkanSemaphore& r);
	~VulkanSemaphore();

	VulkanSemaphore& operator=(const VulkanSemaphore& r);
	VulkanSemaphore& operator=(VulkanSemaphore&& r);

	bool operator!() const
	{
		return VK_NULL_HANDLE == f_Semaphore;
	}
private:
	VulkanSemaphore(VkSemaphore semaphore, VulkanDevice* device);

	VulkanDevice* f_Device;
	VkSemaphore f_Semaphore;

	int* f_Usage;
};
//------------------------------------------------------------------------------
CM_END
//------------------------------------------------------------------------------
#endif//_CM_VULKAN_SEMAPHORE_h_