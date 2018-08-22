/**
	\brief Vulkan队列封装类
*/
//------------------------------------------------------------------------------
#ifndef _CM_VULKAN_QUEUE_h_
#define _CM_VULKAN_QUEUE_h_ "cmVulkanQueue.h"
//------------------------------------------------------------------------------
#include "cmPublic.h"
#include "cmGLFWVulkan.h"
#include "cmVulkanFence.h"
#include "cmVulkanSemaphore.h"
#include <vector>
//------------------------------------------------------------------------------
CM_BEGIN
//------------------------------------------------------------------------------
class VulkanSemaphore;
class VulkanSwapchain;
class VulkanCommandBuffer;
//------------------------------------------------------------------------------
class VulkanQueue
{
public:
	friend class VulkanDevice;

	VulkanQueue();
	VulkanQueue(VulkanQueue& r);
	~VulkanQueue();

	void Submit(
		const VulkanSemaphore* waitSemaphore,
		VkPipelineStageFlags waitDstStageMage,
		const VulkanCommandBuffer& commandBuffer,
		const VulkanSemaphore* semaphore = nullptr,
		const VulkanFence* fence = nullptr) const;

	void Present(
		const VulkanSemaphore* waitSemaphore, 
		const VulkanSwapchain* swapchain,
		uint32_t image) const;

	uint32_t getFamily() const
	{
		return f_Family;
	}

	bool operator!() const
	{
		return VK_NULL_HANDLE == f_Queue;
	}

	bool operator==(const VulkanQueue& r) const
	{
		return f_Queue == r.f_Queue;
	}
private:
	VulkanQueue(VkQueue queue, uint32_t family, VulkanDevice* device);

	VulkanDevice* f_Device;
	uint32_t f_Family;
	VkQueue f_Queue;
};
//------------------------------------------------------------------------------
CM_END
//------------------------------------------------------------------------------
#endif//_CM_VULKAN_QUEUE_h_