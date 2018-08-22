/**
	\brief Vulkan描述符集布局封装类
*/
//------------------------------------------------------------------------------
#ifndef _CM_VULKAN_DESCRIPTOR_SET_LAYOUT_h_
#define _CM_VULKAN_DESCRIPTOR_SET_LAYOUT_h_ "cmVulkanDescriptorSetLayout.h"
//------------------------------------------------------------------------------
#include "cmPublic.h"
#include "cmGLFWVulkan.h"
#include "cmVulkanDescriptorSet.h"
#include <vector>
//------------------------------------------------------------------------------
CM_BEGIN
//------------------------------------------------------------------------------
class VulkanDevice;
//------------------------------------------------------------------------------
class VulkanDescriptorSetLayout
{
public:
	friend class VulkanDevice;

	VulkanDescriptorSetLayout();
	VulkanDescriptorSetLayout(const VulkanDescriptorSetLayout& r);
	~VulkanDescriptorSetLayout();

	VulkanDescriptorSetLayout& operator=(const VulkanDescriptorSetLayout& r);
	VulkanDescriptorSetLayout& operator=(VulkanDescriptorSetLayout&& r);

	VulkanDescriptorSet AllocateDescriptorSet();

	bool operator!() const
	{
		return VK_NULL_HANDLE == f_Layout;
	}
private:
	VulkanDescriptorSetLayout(VkDescriptorSetLayout layout, VkDescriptorPool pool, VulkanDevice* device);

	VulkanDevice* f_Device;
	VkDescriptorSetLayout f_Layout;
	VkDescriptorPool f_Pool;

	int* f_Usage;
};
//------------------------------------------------------------------------------
CM_END
//------------------------------------------------------------------------------
#endif//_CM_VULKAN_DESCRIPTOR_SET_LAYOUT_h_