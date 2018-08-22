/**
	\brief Vulkan½»»»Á´·â×°Àà
*/
//------------------------------------------------------------------------------
#include "cmVulkanDevice.h"
#include "cmVulkanSwapchain.h"
//------------------------------------------------------------------------------
CM_BEGIN
//------------------------------------------------------------------------------
VulkanSwapchain::VulkanSwapchain()
: f_Swapchain(VK_NULL_HANDLE), f_Format(VK_FORMAT_UNDEFINED), f_Size({ 0, 0 }), f_Usage(new int(1)), f_Device(nullptr)
{

}
//------------------------------------------------------------------------------
VulkanSwapchain::VulkanSwapchain(VkSwapchainKHR swapchain, const VkSwapchainCreateInfoKHR& swapchaininfo, VulkanDevice* device)
	: f_Swapchain(swapchain), f_Format(swapchaininfo.imageFormat), f_Size(swapchaininfo.imageExtent), f_Usage(new int(1)), f_Device(device)
{
	GetImages();
	CreateViews();
}
//------------------------------------------------------------------------------
VulkanSwapchain::VulkanSwapchain(VulkanSwapchain& r)
	: f_Swapchain(r.f_Swapchain), f_Format(r.f_Format), f_Size(r.f_Size), f_Images(r.f_Images), f_ImageViews(r.f_ImageViews), f_Usage(r.f_Usage), f_Device(r.f_Device)
{
	++(*f_Usage);
}
//------------------------------------------------------------------------------
VulkanSwapchain::~VulkanSwapchain()
{
	if (0 == --(*f_Usage))
	{
		if (VK_NULL_HANDLE != f_Swapchain)
		{
			f_Device->f_DeviceFunctions.vkDestroySwapchainKHR(f_Device->f_Device, f_Swapchain, nullptr);
		}
		for (auto v : f_ImageViews)
		{
			f_Device->f_DeviceFunctions.vkDestroyImageView(f_Device->f_Device, v, nullptr);
		}
		delete f_Usage;
	}
}
//------------------------------------------------------------------------------
VulkanSwapchain& VulkanSwapchain::operator=(const VulkanSwapchain& r)
{
	if (this != &r)
	{
		if (0 == --(*f_Usage))
		{
			if (VK_NULL_HANDLE != f_Swapchain)
			{
				f_Device->f_DeviceFunctions.vkDestroySwapchainKHR(f_Device->f_Device, f_Swapchain, nullptr);
			}
			delete f_Usage;
		}
		f_Swapchain = r.f_Swapchain;
		f_Format = r.f_Format;
		f_Size = r.f_Size;
		f_Images = r.f_Images;
		f_ImageViews = r.f_ImageViews;
		f_Device = r.f_Device;
		f_Usage = r.f_Usage;
		++(*f_Usage);
	}
	return *this;
}
//------------------------------------------------------------------------------
VulkanSwapchain& VulkanSwapchain::operator=(VulkanSwapchain&& r)
{
	if (this != &r)
	{
		std::swap(f_Swapchain, r.f_Swapchain);
		std::swap(f_Format, r.f_Format);
		std::swap(f_Size, r.f_Size);
		std::swap(f_Images, r.f_Images);
		std::swap(f_ImageViews, r.f_ImageViews);
		std::swap(f_Device, r.f_Device);
		std::swap(f_Usage, r.f_Usage);
	}
	return *this;
}
//------------------------------------------------------------------------------
void VulkanSwapchain::GetImages()
{
	if (VK_NULL_HANDLE != f_Swapchain)
	{
		uint32_t imageCount;
		if ((f_Device->f_DeviceFunctions.vkGetSwapchainImagesKHR(f_Device->f_Device, f_Swapchain, &imageCount, nullptr) != VK_SUCCESS) || (0 == imageCount))
		{
			THROW("Could not get image count of swapchain.");
		}
		f_Images.resize(imageCount);
		if (f_Device->f_DeviceFunctions.vkGetSwapchainImagesKHR(f_Device->f_Device, f_Swapchain, &imageCount, &f_Images[0]) != VK_SUCCESS)
		{
			THROW("Could not get swap chain images!");
		}
	}
}
//------------------------------------------------------------------------------
void VulkanSwapchain::CreateViews()
{
	for (auto i : f_Images)
	{
		VkImageViewCreateInfo image_view_create_info =
		{
			VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,   // VkStructureType                sType
			nullptr,                                    // const void                    *pNext
			0,                                          // VkImageViewCreateFlags         flags
			i,                                          // VkImage                        image
			VK_IMAGE_VIEW_TYPE_2D,                      // VkImageViewType                viewType
			f_Format,                                   // VkFormat                       format
			{                                           // VkComponentMapping             components
				VK_COMPONENT_SWIZZLE_IDENTITY,              // VkComponentSwizzle             r
				VK_COMPONENT_SWIZZLE_IDENTITY,              // VkComponentSwizzle             g
				VK_COMPONENT_SWIZZLE_IDENTITY,              // VkComponentSwizzle             b
				VK_COMPONENT_SWIZZLE_IDENTITY               // VkComponentSwizzle             a
			},
			{                                           // VkImageSubresourceRange        subresourceRange
				VK_IMAGE_ASPECT_COLOR_BIT,                  // VkImageAspectFlags             aspectMask
				0,                                          // uint32_t                       baseMipLevel
				1,                                          // uint32_t                       levelCount
				0,                                          // uint32_t                       baseArrayLayer
				1                                           // uint32_t                       layerCount
			}
		};

		VkImageView v;
		if (f_Device->f_DeviceFunctions.vkCreateImageView(f_Device->f_Device, &image_view_create_info, nullptr, &v) != VK_SUCCESS)
		{
			THROW("Could not create image view for framebuffer!");
		}
		f_ImageViews.push_back(v);
	}
}
//------------------------------------------------------------------------------
uint32_t VulkanSwapchain::AcquireNextImage(const VulkanSemaphore& semaphore) const
{
	uint32_t image;
	switch (f_Device->f_DeviceFunctions.vkAcquireNextImageKHR(f_Device->f_Device, f_Swapchain, UINT64_MAX, semaphore.f_Semaphore, VK_NULL_HANDLE, &image))
	{
	case VK_SUCCESS:
		break;
	case VK_SUBOPTIMAL_KHR:
		throw cmSuboptimal("Error VK_SUBOPTIMAL_KHR while acquire next image.");
		break;
	case VK_ERROR_OUT_OF_DATE_KHR:
		throw cmErrorOutOfDate("Error VK_ERROR_OUT_OF_DATE_KHR while acquire next image.");
		break;
	default:
		THROW("Error while acquire next image.");
	}
	return image;
}
//------------------------------------------------------------------------------
CM_END
