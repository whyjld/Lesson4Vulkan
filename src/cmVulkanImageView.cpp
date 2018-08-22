/**
	\brief Vulkan Í¼ÏñÊÓÍ¼·â×°Àà
*/
//------------------------------------------------------------------------------
#include "cmVulkanDevice.h"
#include "cmVulkanImageView.h"
//------------------------------------------------------------------------------
CM_BEGIN
//------------------------------------------------------------------------------
VulkanImageView::VulkanImageView()
	: f_Device(nullptr), f_ImageView(VK_NULL_HANDLE), f_Usage(new int(1))
{

}
//------------------------------------------------------------------------------
VulkanImageView::VulkanImageView(VkImageView image, const VkImageViewCreateInfo& info, VulkanDevice* device)
	: f_Device(device), f_ImageView(image), f_Info(info), f_Usage(new int(1))
{
}
//------------------------------------------------------------------------------
VulkanImageView::VulkanImageView(VulkanImageView& r)
	: f_Device(r.f_Device), f_ImageView(r.f_ImageView), f_Info(r.f_Info), f_Usage(r.f_Usage)
{
	++(*f_Usage);
}
//------------------------------------------------------------------------------
VulkanImageView::~VulkanImageView()
{
	if (0 == --(*f_Usage))
	{
		if (VK_NULL_HANDLE != f_ImageView)
		{
			f_Device->f_DeviceFunctions.vkDestroyImageView(f_Device->f_Device, f_ImageView, nullptr);
		}
		delete f_Usage;
	}
}
//------------------------------------------------------------------------------
VulkanImageView& VulkanImageView::operator=(const VulkanImageView& r)
{
	if (this != &r)
	{
		if (0 == --(*f_Usage))
		{
			if (VK_NULL_HANDLE != f_ImageView)
			{
				f_Device->f_DeviceFunctions.vkDestroyImageView(f_Device->f_Device, f_ImageView, nullptr);
			}
			delete f_Usage;
		}
		f_Device = r.f_Device;
		f_ImageView = r.f_ImageView;
		f_Info = r.f_Info;
		f_Usage = r.f_Usage;
		++(*f_Usage);
	}
	return *this;
}
//------------------------------------------------------------------------------
VulkanImageView& VulkanImageView::operator=(VulkanImageView&& r)
{
	if (this != &r)
	{
		std::swap(f_Device, r.f_Device);
		std::swap(f_ImageView, r.f_ImageView);
		std::swap(f_Info, r.f_Info);
		std::swap(f_Usage, r.f_Usage);
	}
	return *this;
}
//------------------------------------------------------------------------------
CM_END
