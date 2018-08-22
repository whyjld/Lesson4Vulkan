/**
	\brief Vulkan描述符集布局封装类
*/
//------------------------------------------------------------------------------
#include "cmVulkanDevice.h"
#include "cmVulkanDescriptorSet.h"
#include "cmVulkanDescriptorSetLayout.h"
//------------------------------------------------------------------------------
CM_BEGIN
//------------------------------------------------------------------------------
VulkanDescriptorSetLayout::VulkanDescriptorSetLayout()
	: f_Device(nullptr), f_Layout(VK_NULL_HANDLE), f_Pool(VK_NULL_HANDLE), f_Usage(new int(1))
{

}
//------------------------------------------------------------------------------
VulkanDescriptorSetLayout::VulkanDescriptorSetLayout(VkDescriptorSetLayout layout, VkDescriptorPool pool, VulkanDevice* device)
	: f_Device(device), f_Layout(layout), f_Pool(pool), f_Usage(new int(1))
{
}
//------------------------------------------------------------------------------
VulkanDescriptorSetLayout::VulkanDescriptorSetLayout(const VulkanDescriptorSetLayout& r)
	: f_Device(r.f_Device), f_Layout(r.f_Layout), f_Pool(r.f_Pool), f_Usage(r.f_Usage)
{
	++(*f_Usage);
}
//------------------------------------------------------------------------------
VulkanDescriptorSetLayout::~VulkanDescriptorSetLayout()
{
	if (0 == --(*f_Usage))
	{
		if (VK_NULL_HANDLE != f_Pool)
		{
			f_Device->f_DeviceFunctions.vkDestroyDescriptorPool(f_Device->f_Device, f_Pool, nullptr);
		}
		if (VK_NULL_HANDLE != f_Layout)
		{
			f_Device->f_DeviceFunctions.vkDestroyDescriptorSetLayout(f_Device->f_Device, f_Layout, nullptr);
		}
		delete f_Usage;
	}
}
//------------------------------------------------------------------------------
VulkanDescriptorSetLayout& VulkanDescriptorSetLayout::operator=(const VulkanDescriptorSetLayout& r)
{
	if (this != &r)
	{
		if (0 == --(*f_Usage))
		{
			if (VK_NULL_HANDLE != f_Pool)
			{
				f_Device->f_DeviceFunctions.vkDestroyDescriptorPool(f_Device->f_Device, f_Pool, nullptr);
			}
			if (VK_NULL_HANDLE != f_Layout)
			{
				f_Device->f_DeviceFunctions.vkDestroyDescriptorSetLayout(f_Device->f_Device, f_Layout, nullptr);
			}
			delete f_Usage;
		}
		f_Device = r.f_Device;
		f_Layout = r.f_Layout;
		f_Pool = r.f_Pool;
		f_Usage = r.f_Usage;
		++(*f_Usage);
	}
	return *this;
}
//------------------------------------------------------------------------------
VulkanDescriptorSetLayout& VulkanDescriptorSetLayout::operator=(VulkanDescriptorSetLayout&& r)
{
	if (this != &r)
	{
		std::swap(f_Device, r.f_Device);
		std::swap(f_Layout, r.f_Layout);
		std::swap(f_Pool, r.f_Pool);
		std::swap(f_Usage, r.f_Usage);
	}
	return *this;
}
//------------------------------------------------------------------------------
VulkanDescriptorSet VulkanDescriptorSetLayout::AllocateDescriptorSet()
{
	VkDescriptorSetAllocateInfo descriptor_set_allocate_info =
	{
		VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO, // VkStructureType                sType
		nullptr,                                        // const void                    *pNext
		f_Pool,                                         // VkDescriptorPool               descriptorPool
		1,                                              // uint32_t                       descriptorSetCount
		&f_Layout                                       // const VkDescriptorSetLayout   *pSetLayouts
	};

	VkDescriptorSet ds;
	if (f_Device->f_DeviceFunctions.vkAllocateDescriptorSets(f_Device->f_Device, &descriptor_set_allocate_info, &ds) != VK_SUCCESS)
	{
		THROW("Could not allocate descriptor set!");
	}
	return VulkanDescriptorSet(ds, f_Device);
}
//------------------------------------------------------------------------------
CM_END
