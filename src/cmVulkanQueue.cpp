/**
	\brief Vulkan instance·â×°Àà
*/
//------------------------------------------------------------------------------
#include "cmVulkanInstance.h"
#include "cmVulkanSemaphore.h"
#include "cmVulkanSwapchain.h"
#include "cmVulkanQueue.h"
//------------------------------------------------------------------------------
CM_BEGIN
//------------------------------------------------------------------------------
VulkanQueue::VulkanQueue()
: f_Queue(VK_NULL_HANDLE), f_Family(static_cast<uint32_t>(-1)), f_Device(nullptr)
{

}
//------------------------------------------------------------------------------
VulkanQueue::VulkanQueue(VkQueue queue, uint32_t family, VulkanDevice* device)
: f_Queue(queue), f_Family(family), f_Device(device)
{
}
//------------------------------------------------------------------------------
VulkanQueue::VulkanQueue(VulkanQueue& r)
	: f_Queue(r.f_Queue), f_Family(r.f_Family), f_Device(r.f_Device)
{
}
//------------------------------------------------------------------------------
VulkanQueue::~VulkanQueue()
{
}
//------------------------------------------------------------------------------
void VulkanQueue::Submit(const VulkanSemaphore* waitSemaphore,
	VkPipelineStageFlags waitDstStageMage,
	const VulkanCommandBuffer& commandBuffer,
	const VulkanSemaphore* signalSemaphore,
	const VulkanFence* fence) const
{
	VkSubmitInfo submit_info =
	{
		VK_STRUCTURE_TYPE_SUBMIT_INFO,                  // VkStructureType              sType
		nullptr,                                        // const void                  *pNext
		(waitSemaphore != nullptr) ? 1 : 0,             // uint32_t                     waitSemaphoreCount
		(waitSemaphore != nullptr) ? &waitSemaphore->f_Semaphore : nullptr, // const VkSemaphore           *pWaitSemaphores
		&waitDstStageMage,                              // const VkPipelineStageFlags  *pWaitDstStageMask;
		1,                                              // uint32_t                     commandBufferCount
		&commandBuffer.f_Buffer,                        // const VkCommandBuffer       *pCommandBuffers
		(nullptr == signalSemaphore) ? 0 : 1,                       // uint32_t                     signalSemaphoreCount
		(nullptr == signalSemaphore)  ? nullptr : &signalSemaphore->f_Semaphore, // const VkSemaphore           *pSignalSemaphores
	};

	if (f_Device->f_DeviceFunctions.vkQueueSubmit(f_Queue, 1, &submit_info, (nullptr == fence) ? VK_NULL_HANDLE : fence->f_Fence) != VK_SUCCESS)
	{
		THROW("Error while submit queue.");
	}
}
//------------------------------------------------------------------------------
void VulkanQueue::Present(
	const VulkanSemaphore* waitSemaphore,
	const VulkanSwapchain* swapchain,
	uint32_t image) const
{
	VkPresentInfoKHR presentInfo =
	{
		VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,           // VkStructureType              sType
		nullptr,                                      // const void                  *pNext
		(waitSemaphore != nullptr) ? 1 : 0,           // uint32_t                     waitSemaphoreCount
		(waitSemaphore != nullptr) ? &waitSemaphore->f_Semaphore : nullptr, // const VkSemaphore           *pWaitSemaphores
		1,                                            // uint32_t                     swapchainCount
		&(swapchain->f_Swapchain),                    // const VkSwapchainKHR        *pSwapchains
		&image,                                       // const uint32_t              *pImageIndices
		nullptr                                       // VkResult                    *pResults
	};
	switch (f_Device->f_DeviceFunctions.vkQueuePresentKHR(f_Queue, &presentInfo))
	{
	case VK_SUCCESS:
		break;
	case VK_SUBOPTIMAL_KHR:
		throw cmSuboptimal("Error while acquire next image.");
		break;
	case VK_ERROR_OUT_OF_DATE_KHR:
		throw cmErrorOutOfDate("Error while acquire next image.");
		break;
	default:
		THROW("Error while acquire next image.");
	}
}
//------------------------------------------------------------------------------
CM_END
