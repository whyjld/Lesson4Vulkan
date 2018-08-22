/**
	\brief VulkanÃüÁî»º³å·â×°Àà
*/
//------------------------------------------------------------------------------
#include "cmVulkanCommandBuffer.h"
#include "cmVulkanGraphicsPipeline.h"
#include "cmVulkanPipelineLayout.h"
#include "cmVulkanDescriptorSet.h"
#include "cmVulkanFrameBuffer.h"
#include "cmVulkanRenderPass.h"
#include "cmVulkanBuffer.h"
#include "cmVulkanImage.h"
#include "cmVulkanInstance.h"
//------------------------------------------------------------------------------
CM_BEGIN
//------------------------------------------------------------------------------
VulkanCommandBuffer::VulkanCommandBuffer()
	: f_Device(nullptr), f_Pool(VK_NULL_HANDLE), f_Buffer(VK_NULL_HANDLE), f_Usage(new int(1))
{

}
//------------------------------------------------------------------------------
VulkanCommandBuffer::VulkanCommandBuffer(VkCommandPool pool, VulkanDevice* device)
	: f_Device(device), f_Pool(pool), f_Buffer(VK_NULL_HANDLE), f_Usage(new int(1))
{
	VkCommandBufferAllocateInfo commandBufferAllocateInfo =
	{
		VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO, // VkStructureType              sType
		nullptr,                                        // const void*                  pNext
		f_Pool,                                         // VkCommandPool                commandPool
		VK_COMMAND_BUFFER_LEVEL_PRIMARY,                // VkCommandBufferLevel         level
		1,                                              // uint32_t                     bufferCount
	};
	if (f_Device->f_DeviceFunctions.vkAllocateCommandBuffers(f_Device->f_Device, &commandBufferAllocateInfo, &f_Buffer) != VK_SUCCESS)
	{
		THROW("Could not allocate command buffers!");
	}
}
//------------------------------------------------------------------------------
VulkanCommandBuffer::VulkanCommandBuffer(VulkanCommandBuffer& r)
	: f_Device(r.f_Device), f_Pool(r.f_Pool), f_Buffer(r.f_Buffer), f_Usage(r.f_Usage)
{
	++(*f_Usage);
}
//------------------------------------------------------------------------------
VulkanCommandBuffer::~VulkanCommandBuffer()
{
	if (0 == --*(f_Usage))
	{
		if (f_Buffer != VK_NULL_HANDLE)
		{
			f_Device->f_DeviceFunctions.vkFreeCommandBuffers(f_Device->f_Device, f_Pool, 1, &f_Buffer);
		}
	}
}
//------------------------------------------------------------------------------
VulkanCommandBuffer& VulkanCommandBuffer::operator=(const VulkanCommandBuffer& r)
{
	if (this != &r)
	{
		if (0 == --*(f_Usage))
		{
			if (f_Buffer != VK_NULL_HANDLE)
			{
				f_Device->f_DeviceFunctions.vkFreeCommandBuffers(f_Device->f_Device, f_Pool, 1, &f_Buffer);
			}
		}
		f_Device = r.f_Device;
		f_Pool = r.f_Pool;
		f_Buffer = r.f_Buffer;
		f_Usage = r.f_Usage;
		++(*f_Usage);
	}
	return *this;
}
//------------------------------------------------------------------------------
VulkanCommandBuffer& VulkanCommandBuffer::operator=(VulkanCommandBuffer&& r)
{
	if (this != &r)
	{
		std::swap(f_Device, r.f_Device);
		std::swap(f_Pool, r.f_Pool);
		std::swap(f_Buffer, r.f_Buffer);
		std::swap(f_Usage, r.f_Usage);
	}
	return *this;
}
//------------------------------------------------------------------------------
void VulkanCommandBuffer::BeginCommandBuffer(VkCommandBufferUsageFlags flags)
{
	VkCommandBufferBeginInfo command_buffer_begin_info =
	{
		VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,      // VkStructureType                        sType
		nullptr,                                          // const void                            *pNext
		flags,                                            // VkCommandBufferUsageFlags              flags
		nullptr                                           // const VkCommandBufferInheritanceInfo  *pInheritanceInfo
	};
	if (VK_SUCCESS != f_Device->f_DeviceFunctions.vkBeginCommandBuffer(f_Buffer, &command_buffer_begin_info))
	{
		THROW("Could not record "__FUNCTION__" buffers.");
	}
}
//------------------------------------------------------------------------------
void VulkanCommandBuffer::BeginRenderPass(
	const VulkanRenderPass& renderpass,
	const VulkanFramebuffer& framebuffer,
	const VkRect2D& viewport,
	const VkClearValue& clearvalue
	)
{
	VkRenderPassBeginInfo render_pass_begin_info = {
		VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,     // VkStructureType                sType
		nullptr,                                      // const void                    *pNext
		renderpass.f_RenderPass,                      // VkRenderPass                   renderPass
		framebuffer.f_Framebuffer,                    // VkFramebuffer                  framebuffer
		viewport,                                     // VkRect2D                       renderArea
		1,                                            // uint32_t                       clearValueCount
		&clearvalue                                   // const VkClearValue            *pClearValues
	};
	f_Device->f_DeviceFunctions.vkCmdBeginRenderPass(f_Buffer, &render_pass_begin_info, VK_SUBPASS_CONTENTS_INLINE);
}
//------------------------------------------------------------------------------
void VulkanCommandBuffer::BindPipeline(const VulkanGraphicsPipeline& pipeline)
{
	f_Device->f_DeviceFunctions.vkCmdBindPipeline(f_Buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline.f_Pipeline);
}
//------------------------------------------------------------------------------
void VulkanCommandBuffer::BindDescriptorSets(const VulkanPipelineLayout& layout, const std::vector<CM::VulkanDescriptorSet>& sets)
{
	std::vector<VkDescriptorSet> dss;
	for (const auto& s : sets)
	{
		dss.push_back(s.f_DescriptorSet);
	}
	f_Device->f_DeviceFunctions.vkCmdBindDescriptorSets(f_Buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, layout.f_PipelineLayout, 0, dss.size(), &dss[0], 0, nullptr);
}
//------------------------------------------------------------------------------
void VulkanCommandBuffer::PipelineBarrier(
	VkPipelineStageFlags         srcStageMask,
	VkPipelineStageFlags         dstStageMask,
	VkDependencyFlags            dependencyFlags,
	uint32_t                     memoryBarrierCount,
	const VkMemoryBarrier*       pMemoryBarriers,
	uint32_t                     bufferMemoryBarrierCount,
	const VkBufferMemoryBarrier* pBufferMemoryBarriers,
	uint32_t                     imageMemoryBarrierCount,
	const VkImageMemoryBarrier*  pImageMemoryBarriers)
{
	f_Device->f_DeviceFunctions.vkCmdPipelineBarrier(
		f_Buffer,
		srcStageMask,
		dstStageMask,
		dependencyFlags,
		memoryBarrierCount,
		pMemoryBarriers,
		bufferMemoryBarrierCount,
		pBufferMemoryBarriers,
		imageMemoryBarrierCount,
		pImageMemoryBarriers);
}
//------------------------------------------------------------------------------
void VulkanCommandBuffer::ClearColorImage(
	VkImage image,
	VkImageLayout imageLayout,
	const VkClearColorValue* pColor,
	uint32_t rangeCount,
	const VkImageSubresourceRange* pRanges)
{
	f_Device->f_DeviceFunctions.vkCmdClearColorImage(
		f_Buffer, 
		image, 
		imageLayout, 
		pColor, 
		rangeCount, 
		pRanges);
}
//------------------------------------------------------------------------------
void VulkanCommandBuffer::BindVertexBuffers(uint32_t firstBinding, std::vector<CM::BufferInfo> buffers)
{
	std::vector<VkBuffer> bufs;
	std::vector<VkDeviceSize> offs;

	for (auto& b : buffers)
	{
		bufs.push_back(b.Buffer.f_Buffer);
		offs.push_back(b.Offset);
	}

	f_Device->f_DeviceFunctions.vkCmdBindVertexBuffers(
		f_Buffer,
		firstBinding,
		buffers.size(),
		&bufs[0],
		&offs[0]);
}
//------------------------------------------------------------------------------
void VulkanCommandBuffer::Draw(
	uint32_t vertexCount,
	uint32_t instanceCount,
	uint32_t firstVertex,
	uint32_t firstInstance)
{
	f_Device->f_DeviceFunctions.vkCmdDraw(
		f_Buffer,
		vertexCount,
		instanceCount,
		firstVertex,
		firstInstance);
}
//------------------------------------------------------------------------------
void VulkanCommandBuffer::CopyBuffer(
	const VulkanBuffer& srcBuffer,
	const VulkanBuffer& dstBuffer,
	const std::vector<VkBufferCopy>& regions)
{
	f_Device->f_DeviceFunctions.vkCmdCopyBuffer(f_Buffer, srcBuffer.f_Buffer, dstBuffer.f_Buffer, regions.size(), &regions[0]);
}
//------------------------------------------------------------------------------
void VulkanCommandBuffer::CopyImage(
	const VulkanBuffer& srcBuffer,
	const VulkanImage& dstImage,
	const std::vector<VkBufferImageCopy>& regions)
{
	f_Device->f_DeviceFunctions.vkCmdCopyBufferToImage(f_Buffer, srcBuffer.f_Buffer, dstImage.f_Image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, regions.size(), &regions[0]);
}
//------------------------------------------------------------------------------
void VulkanCommandBuffer::EndRenderPass()
{
	f_Device->f_DeviceFunctions.vkCmdEndRenderPass(f_Buffer);
}
//------------------------------------------------------------------------------
void VulkanCommandBuffer::EndCommandBuffer()
{
	if (VK_SUCCESS != f_Device->f_DeviceFunctions.vkEndCommandBuffer(f_Buffer))
	{
		THROW("Could not record "__FUNCTION__" buffers.");
	}
}
//------------------------------------------------------------------------------
void VulkanCommandBuffer::ResetCommandBuffer(VkCommandBufferResetFlags flags)
{
	if (VK_SUCCESS != f_Device->f_DeviceFunctions.vkResetCommandBuffer(f_Buffer, flags))
	{
		THROW("Could not record "__FUNCTION__" buffers.");
	}
}
//------------------------------------------------------------------------------
CM_END
