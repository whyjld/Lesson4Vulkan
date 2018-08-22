/**
	\brief Vulkan½»»»Á´·â×°Àà
*/
//------------------------------------------------------------------------------
#ifndef _CM_VULKAN_SWAPCHAIN_h_
#define _CM_VULKAN_SWAPCHAIN_h_ "cmVulkanSwapchain.h"
//------------------------------------------------------------------------------
#include "cmPublic.h"
#include "cmGLFWVulkan.h"
//------------------------------------------------------------------------------
CM_BEGIN
//------------------------------------------------------------------------------
class VulkanSemaphore;
//------------------------------------------------------------------------------
class VulkanSwapchain
{
public:
	friend class VulkanDevice;
	friend class VulkanQueue;

	VulkanSwapchain();
	VulkanSwapchain(VulkanSwapchain& r);
	~VulkanSwapchain();

	VulkanSwapchain& operator=(const VulkanSwapchain& r);
	VulkanSwapchain& operator=(VulkanSwapchain&& r);

	VkFormat getFormat() const
	{
		return f_Format;
	}

	VkExtent2D getSize() const
	{
		return f_Size;
	}

	uint32_t getImageCount() const
	{
		return f_Images.size();
	}
	const std::vector<VkImage>& getImages() const
	{
		return f_Images;
	}
	const std::vector<VkImageView> getImageViews() const
	{
		return f_ImageViews;
	}

	uint32_t AcquireNextImage(const VulkanSemaphore& semaphore) const;

	bool operator!() const
	{
		return VK_NULL_HANDLE == f_Swapchain;
	}
private:
	VulkanSwapchain(VkSwapchainKHR swapchain, const VkSwapchainCreateInfoKHR& swapchaininfo, VulkanDevice* device);
	void GetImages();
	void CreateViews();

	VulkanDevice* f_Device;
	VkSwapchainKHR f_Swapchain;
	VkFormat f_Format;
	VkExtent2D f_Size;

	std::vector<VkImage> f_Images;
	std::vector<VkImageView> f_ImageViews;

	int* f_Usage;
};
//------------------------------------------------------------------------------
CM_END
//------------------------------------------------------------------------------
#endif//_CM_VULKAN_SWAPCHAIN_h_