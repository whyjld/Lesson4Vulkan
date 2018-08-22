/**
	\brief Vulkan Í¼Ïñ·â×°Àà
*/
//------------------------------------------------------------------------------
#include "cmVulkanDevice.h"
#include "cmVulkanImage.h"
#include "cmVulkanImageView.h"
//------------------------------------------------------------------------------
CM_BEGIN
//------------------------------------------------------------------------------
VulkanImage::VulkanImage()
	: f_Device(nullptr), f_Image(VK_NULL_HANDLE), f_Offset(~VkDeviceSize(0)), f_Usage(new int(1))
{

}
//------------------------------------------------------------------------------
VulkanImage::VulkanImage(VkImage image, const VkImageCreateInfo& info, VulkanDevice* device)
	: f_Device(device), f_Image(image), f_Info(info), f_Offset(~VkDeviceSize(0)), f_Usage(new int(1))
{
}
//------------------------------------------------------------------------------
VulkanImage::VulkanImage(VulkanImage& r)
	: f_Device(r.f_Device), f_Image(r.f_Image), f_Info(r.f_Info), f_Memory(r.f_Memory), f_Offset(r.f_Offset), f_Usage(r.f_Usage)
{
	++(*f_Usage);
}
//------------------------------------------------------------------------------
VulkanImage::~VulkanImage()
{
	if (0 == --(*f_Usage))
	{
		if (VK_NULL_HANDLE != f_Image)
		{
			f_Device->f_DeviceFunctions.vkDestroyImage(f_Device->f_Device, f_Image, nullptr);
		}
		delete f_Usage;
	}
}
//------------------------------------------------------------------------------
VulkanImage& VulkanImage::operator=(const VulkanImage& r)
{
	if (this != &r)
	{
		if (0 == --(*f_Usage))
		{
			if (VK_NULL_HANDLE != f_Image)
			{
				f_Device->f_DeviceFunctions.vkDestroyImage(f_Device->f_Device, f_Image, nullptr);
			}
			delete f_Usage;
		}
		f_Device = r.f_Device;
		f_Image = r.f_Image;
		f_Info = r.f_Info;
		f_Memory = r.f_Memory;
		f_Offset = r.f_Offset;
		f_Usage = r.f_Usage;
		++(*f_Usage);
	}
	return *this;
}
//------------------------------------------------------------------------------
VulkanImage& VulkanImage::operator=(VulkanImage&& r)
{
	if (this != &r)
	{
		std::swap(f_Device, r.f_Device);
		std::swap(f_Image, r.f_Image);
		std::swap(f_Info, r.f_Info);
		std::swap(f_Memory, r.f_Memory);
		std::swap(f_Offset, r.f_Offset);
		std::swap(f_Usage, r.f_Usage);
	}
	return *this;
}
//------------------------------------------------------------------------------
VkImageMemoryBarrier VulkanImage::CreateImageMemoryBarrier(uint32_t baseMipLevel, uint32_t levelCount, uint32_t baseArrayLayer, uint32_t layerCount, VkAccessFlags srcAccessMask, VkAccessFlags dstAccessMask, VkImageLayout oldLayout, VkImageLayout newLayout)
{
	return VkImageMemoryBarrier(
	{
		VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,             // VkStructureType                        sType
		nullptr,                                            // const void                            *pNext
		srcAccessMask,                                      // VkAccessFlags                          srcAccessMask
		dstAccessMask,                                      // VkAccessFlags                          dstAccessMask
		oldLayout,                                          // VkImageLayout                          oldLayout
		newLayout,                                          // VkImageLayout                          newLayout
		VK_QUEUE_FAMILY_IGNORED,                            // uint32_t                               srcQueueFamilyIndex
		VK_QUEUE_FAMILY_IGNORED,                            // uint32_t                               dstQueueFamilyIndex
		f_Image,                                            // VkImage                                image
		{                                                   // VkImageSubresourceRange                subresourceRange
			VK_IMAGE_ASPECT_COLOR_BIT,                          // VkImageAspectFlags                     aspectMask
			baseMipLevel,                                       // uint32_t                               baseMipLevel
			levelCount,                                         // uint32_t                               levelCount
			baseArrayLayer,                                     // uint32_t                               baseArrayLayer
			layerCount,                                         // uint32_t                               layerCount
		}
	});
}
//------------------------------------------------------------------------------
VulkanImageView VulkanImage::CreateImageView(VkImageViewType type, uint32_t baseMipLevel, uint32_t levelCount, uint32_t baseArrayLayer, uint32_t layerCount)
{
	VkImageViewCreateInfo image_view_create_info =
	{
		VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO, // VkStructureType          sType
		nullptr,                                  // const void              *pNext
		0,                                        // VkImageViewCreateFlags   flags
		f_Image,                                  // VkImage                  image
		type,                                     // VkImageViewType          viewType
		f_Info.format,                            // VkFormat                 format
		{                                         // VkComponentMapping       components
			VK_COMPONENT_SWIZZLE_IDENTITY,            // VkComponentSwizzle       r
			VK_COMPONENT_SWIZZLE_IDENTITY,            // VkComponentSwizzle       g
			VK_COMPONENT_SWIZZLE_IDENTITY,            // VkComponentSwizzle       b
			VK_COMPONENT_SWIZZLE_IDENTITY             // VkComponentSwizzle       a
		},
		{                                         // VkImageSubresourceRange  subresourceRange
			VK_IMAGE_ASPECT_COLOR_BIT,            // VkImageAspectFlags       aspectMask
			baseMipLevel,                         // uint32_t                 baseMipLevel
			levelCount,                           // uint32_t                 levelCount
			baseArrayLayer,                       // uint32_t                 baseArrayLayer
			layerCount,                           // uint32_t                 layerCount
		}
	};

	VkImageView view;
	if (f_Device->f_DeviceFunctions.vkCreateImageView(f_Device->f_Device, &image_view_create_info, nullptr, &view) != VK_SUCCESS)
	{
		THROW("Could not create image view.");
	}
	return VulkanImageView(view, image_view_create_info, f_Device);
}
//------------------------------------------------------------------------------
bool VulkanImage::innerBind(const VulkanMemory& memory, VkDeviceSize offset)
{
	if (f_Device->f_DeviceFunctions.vkBindImageMemory(f_Device->f_Device, f_Image, memory.f_Memory, offset) == VK_SUCCESS)
	{
		f_Memory = memory;
		f_Offset = offset;
		return true;
	}
	return false;
}
//------------------------------------------------------------------------------
CM_END
