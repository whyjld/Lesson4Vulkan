/**
	\brief VulkanÕºœÒ ”Õº∑‚◊∞¿‡
*/
//------------------------------------------------------------------------------
#ifndef _CM_VULKAN_IMAGE_VIEW_h_
#define _CM_VULKAN_IMAGE_VIEW_h_ "cmVulkanImageViewView.h"
//------------------------------------------------------------------------------
#include "cmPublic.h"
#include "cmGLFWVulkan.h"
#include <vector>
//------------------------------------------------------------------------------
CM_BEGIN
//------------------------------------------------------------------------------
class VulkanDevice;
//------------------------------------------------------------------------------
class VulkanImageView
{
public:
	friend class VulkanImage;
	friend class VulkanDescriptorSet;

	VulkanImageView();
	VulkanImageView(VulkanImageView& r);
	~VulkanImageView();

	VulkanImageView& operator=(const VulkanImageView& r);
	VulkanImageView& operator=(VulkanImageView&& r);

	bool operator!() const
	{
		return VK_NULL_HANDLE == f_ImageView;
	}
private:
	VulkanImageView(VkImageView view, const VkImageViewCreateInfo& info, VulkanDevice* device);

	VulkanDevice* f_Device;
	VkImageView f_ImageView;
	VkImageViewCreateInfo f_Info;

	int* f_Usage;
};
//------------------------------------------------------------------------------
CM_END
//------------------------------------------------------------------------------
#endif//_CM_VULKAN_IMAGE_h_