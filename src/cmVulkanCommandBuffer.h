/**
	\brief Vulkan队列封装类
*/
//------------------------------------------------------------------------------
#ifndef _CM_VULKAN_COMMAND_BUFFER_h_
#define _CM_VULKAN_COMMAND_BUFFER_h_ "cmVulkanCommandBuffer.h"
//------------------------------------------------------------------------------
#include "cmPublic.h"
#include "cmGLFWVulkan.h"
#include <vector>
//------------------------------------------------------------------------------
CM_BEGIN
//------------------------------------------------------------------------------
struct BufferInfo;
//------------------------------------------------------------------------------
class VulkanImage;
class VulkanBuffer;
class VulkanRenderPass;
class VulkanFramebuffer;
class VulkanDescriptorSet;
class VulkanPipelineLayout;
class VulkanGraphicsPipeline;
//------------------------------------------------------------------------------
class VulkanCommandBuffer
{
public:
	friend class VulkanDevice;
	friend class VulkanQueue;

	VulkanCommandBuffer();
	VulkanCommandBuffer(VulkanCommandBuffer& r);
	~VulkanCommandBuffer();

	VulkanCommandBuffer& operator=(const VulkanCommandBuffer& r);
	VulkanCommandBuffer& operator=(VulkanCommandBuffer&& r);

	void BeginCommandBuffer(VkCommandBufferUsageFlags flags);

	void BeginRenderPass(
		const VulkanRenderPass& renderpass,
		const VulkanFramebuffer& framebuffer,
		const VkRect2D& viewport,
		const VkClearValue& clearvalue);

	void BindPipeline(const VulkanGraphicsPipeline& pipeline);

	void BindDescriptorSets(const VulkanPipelineLayout& layout, const std::vector<VulkanDescriptorSet>& sets);

	void PipelineBarrier(
		VkPipelineStageFlags srcStageMask,
		VkPipelineStageFlags                        dstStageMask,
		VkDependencyFlags                           dependencyFlags,
		uint32_t                                    memoryBarrierCount,
		const VkMemoryBarrier*                      pMemoryBarriers,
		uint32_t                                    bufferMemoryBarrierCount,
		const VkBufferMemoryBarrier*                pBufferMemoryBarriers,
		uint32_t                                    imageMemoryBarrierCount,
		const VkImageMemoryBarrier*                 pImageMemoryBarriers);

	void ClearColorImage(
		VkImage image,
		VkImageLayout imageLayout,
		const VkClearColorValue* pColor,
		uint32_t rangeCount,
		const VkImageSubresourceRange* pRanges);

	void BindVertexBuffers(
		uint32_t firstBinding,
		std::vector<CM::BufferInfo> buffers);

	void Draw(
		uint32_t vertexCount,
		uint32_t instanceCount,
		uint32_t firstVertex,
		uint32_t firstInstance);

	void CopyBuffer(
		const VulkanBuffer& srcBuffer,
		const VulkanBuffer& dstBuffer,
		const std::vector<VkBufferCopy>& regions);

	void CopyImage(
		const VulkanBuffer& srcBuffer,
		const VulkanImage& dstImage,
		const std::vector<VkBufferImageCopy>& regions);

	void EndRenderPass();

	void EndCommandBuffer();

	void ResetCommandBuffer(VkCommandBufferResetFlags flags);

	bool operator!() const
	{
		return VK_NULL_HANDLE == f_Buffer;
	}
private:
	VulkanCommandBuffer(VkCommandPool pool, VulkanDevice* device);

	VulkanDevice* f_Device;
	VkCommandPool f_Pool;
	VkCommandBuffer f_Buffer;
	int* f_Usage;
};
//------------------------------------------------------------------------------
CM_END
//------------------------------------------------------------------------------
#endif//_CM_VULKAN_COMMAND_BUFFER_h_