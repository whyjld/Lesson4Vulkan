/**
	\brief VulkanÍ¼Ïñ·â×°Àà
*/
//------------------------------------------------------------------------------
#ifndef _CM_VULKAN_IMAGE_h_
#define _CM_VULKAN_IMAGE_h_ "cmVulkanImage.h"
//------------------------------------------------------------------------------
#include "cmPublic.h"
#include "cmGLFWVulkan.h"
#include "cmVulkanMemory.h"
#include "cmVulkanImageView.h"
#include <vector>
//------------------------------------------------------------------------------
CM_BEGIN
//------------------------------------------------------------------------------
struct ImageInfo
{
	VulkanImage& Image;
	VkDeviceSize Offset;
};
//------------------------------------------------------------------------------
class VulkanImage
{
public:
	friend class VulkanDevice;
	friend class VulkanCommandBuffer;

	VulkanImage();
	VulkanImage(VulkanImage& r);
	~VulkanImage();

	VulkanImage& operator=(const VulkanImage& r);
	VulkanImage& operator=(VulkanImage&& r);

	VkImageMemoryBarrier CreateImageMemoryBarrier(uint32_t baseMipLevel, uint32_t levelCount, uint32_t baseArrayLayer, uint32_t layerCount, VkAccessFlags srcAccessMask, VkAccessFlags dstAccessMask, VkImageLayout oldLayout, VkImageLayout newLayout);

	VulkanImageView CreateImageView(VkImageViewType type, uint32_t baseMipLevel, uint32_t levelCount, uint32_t baseArrayLayer, uint32_t layerCount);

	bool operator!() const
	{
		return VK_NULL_HANDLE == f_Image;
	}
private:
	VulkanImage(VkImage image, const VkImageCreateInfo& info, VulkanDevice* device);
	bool innerBind(const VulkanMemory& memory, VkDeviceSize offset);

	VulkanDevice* f_Device;
	VkImage f_Image;
	VkImageCreateInfo f_Info;
	VulkanMemory f_Memory;
	VkDeviceSize f_Offset;

	int* f_Usage;
};
//------------------------------------------------------------------------------
CM_END
//------------------------------------------------------------------------------
#endif//_CM_VULKAN_IMAGE_h_