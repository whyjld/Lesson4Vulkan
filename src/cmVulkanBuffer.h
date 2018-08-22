/**
	\brief Vulkan»º³å·â×°Àà
*/
//------------------------------------------------------------------------------
#ifndef _CM_VULKAN_BUFFER_h_
#define _CM_VULKAN_BUFFER_h_ "cmVulkanBuffer.h"
//------------------------------------------------------------------------------
#include "cmPublic.h"
#include "cmGLFWVulkan.h"
#include "cmVulkanMemory.h"
#include <vector>
//------------------------------------------------------------------------------
CM_BEGIN
//------------------------------------------------------------------------------
struct BufferInfo
{
	VulkanBuffer& Buffer;
	VkDeviceSize Offset;
};
//------------------------------------------------------------------------------
class VulkanBuffer
{
public:
	friend class VulkanDevice;
	friend class VulkanDescriptorSet;
	friend class VulkanCommandBuffer;

	VulkanBuffer();
	VulkanBuffer(const VulkanBuffer& r);
	~VulkanBuffer();

	VulkanBuffer& operator=(const VulkanBuffer& r);
	VulkanBuffer& operator=(VulkanBuffer&& r);

	VkBufferMemoryBarrier CreateBufferMemoryBarrier(VkAccessFlags srcAccessMask,
		VkAccessFlags dstAccessMask,
		uint32_t srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
		uint32_t dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
		VkDeviceSize offset = 0,
		VkDeviceSize size = VK_WHOLE_SIZE);

	VkDeviceSize Bind(const VulkanMemory& memory, VkDeviceSize offset);

	VulkanMemory& getMemory()
	{
		return f_Memory;
	}

	VkDeviceSize getOffset() const
	{
		return f_Offset;
	}

	VkDeviceSize getSize() const
	{
		return f_Size;
	}

	bool operator!() const
	{
		return VK_NULL_HANDLE == f_Buffer;
	}
private:
	VulkanBuffer(VkBuffer buffer, VulkanDevice* device);
	bool innerBind(const VulkanMemory& memory, VkDeviceSize offset);

	VulkanDevice* f_Device;
	VkBuffer f_Buffer;
	VkDeviceSize f_Size;
	VulkanMemory f_Memory;
	VkDeviceSize f_Offset;

	int* f_Usage;
};
//------------------------------------------------------------------------------
CM_END
//------------------------------------------------------------------------------
#endif//_CM_VULKAN_BUFFER_h_