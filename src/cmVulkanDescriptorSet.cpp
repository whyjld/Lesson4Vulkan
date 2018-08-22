/**
	\brief VulkanÃèÊö·û¼¯·â×°Àà
*/
//------------------------------------------------------------------------------
#include "cmVulkanDevice.h"
#include "cmVulkanSampler.h"
#include "cmVulkanImageView.h"
#include "cmVulkanDescriptorSet.h"
//------------------------------------------------------------------------------
CM_BEGIN
//------------------------------------------------------------------------------
VulkanDescriptorSet::VulkanDescriptorSet()
	: f_Device(nullptr), f_DescriptorSet(VK_NULL_HANDLE), f_Usage(new int(1))
{

}
//------------------------------------------------------------------------------
VulkanDescriptorSet::VulkanDescriptorSet(VkDescriptorSet ds, VulkanDevice* device)
	: f_Device(device), f_DescriptorSet(ds), f_Usage(new int(1))
{
}
//------------------------------------------------------------------------------
VulkanDescriptorSet::VulkanDescriptorSet(VulkanDescriptorSet& r)
	: f_Device(r.f_Device), f_DescriptorSet(r.f_DescriptorSet), f_Usage(r.f_Usage)
{
	++(*f_Usage);
}
//------------------------------------------------------------------------------
VulkanDescriptorSet::~VulkanDescriptorSet()
{
	if (0 == --(*f_Usage))
	{
		if (VK_NULL_HANDLE != f_DescriptorSet)
		{
			//f_Device->f_DeviceFunctions.vkDestroyDescriptorSetLayout(f_Device->f_Device, f_DescriptorSet, nullptr);
		}
		delete f_Usage;
	}
}
//------------------------------------------------------------------------------
VulkanDescriptorSet& VulkanDescriptorSet::operator=(const VulkanDescriptorSet& r)
{
	if (this != &r)
	{
		if (0 == --(*f_Usage))
		{
			if (VK_NULL_HANDLE != f_DescriptorSet)
			{
				f_Device->f_DeviceFunctions.vkDestroyDescriptorSetLayout(f_Device->f_Device, f_DescriptorSet, nullptr);
			}
			delete f_Usage;
		}
		f_Device = r.f_Device;
		f_DescriptorSet = r.f_DescriptorSet;
		f_Usage = r.f_Usage;
		++(*f_Usage);
	}
	return *this;
}
//------------------------------------------------------------------------------
VulkanDescriptorSet& VulkanDescriptorSet::operator=(VulkanDescriptorSet&& r)
{
	if (this != &r)
	{
		std::swap(f_Device, r.f_Device);
		std::swap(f_DescriptorSet, r.f_DescriptorSet);
		std::swap(f_Usage, r.f_Usage);
	}
	return *this;
}
//------------------------------------------------------------------------------
void VulkanDescriptorSet::Update(uint32_t binding, const std::vector<CombinedImageSampler>& combinedImageSamplers)
{
	std::vector<VkDescriptorImageInfo> image_infos;
	for (const auto& cis : combinedImageSamplers)
	{
		image_infos.push_back(VkDescriptorImageInfo(
		{
			cis.Sampler->f_Sampler,                      // VkSampler                      sampler
			cis.View->f_ImageView,                       // VkImageView                    imageView
			VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL    // VkImageLayout                  imageLayout
		}));
	}

	VkWriteDescriptorSet descriptor_writes =
	{
		VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,     // VkStructureType                sType
		nullptr,                                    // const void                    *pNext
		f_DescriptorSet,                            // VkDescriptorSet                dstSet
		binding,                                    // uint32_t                       dstBinding
		0,                                          // uint32_t                       dstArrayElement
		image_infos.size(),                         // uint32_t                       descriptorCount
		VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,  // VkDescriptorType               descriptorType
		&image_infos[0],                            // const VkDescriptorImageInfo   *pImageInfo
		nullptr,                                    // const VkDescriptorBufferInfo  *pBufferInfo
		nullptr                                     // const VkBufferView            *pTexelBufferView
	};

	f_Device->f_DeviceFunctions.vkUpdateDescriptorSets(f_Device->f_Device, 1, &descriptor_writes, 0, nullptr);
}
//------------------------------------------------------------------------------
void VulkanDescriptorSet::Update(uint32_t binding, const std::vector<BufferInfo>& buffers)
{
	std::vector<VkDescriptorBufferInfo> buffer_infos;
	for (const auto& b : buffers)
	{
		buffer_infos.push_back(VkDescriptorBufferInfo(
		{
			b.Buffer->f_Buffer,                              // VkBuffer                       buffer
			b.Offset,                                       // VkDeviceSize                   offset
			b.Range                                         // VkDeviceSize                   range
		}));
	}

	VkWriteDescriptorSet descriptor_writes =
	{
		VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,     // VkStructureType                sType
		nullptr,                                    // const void                    *pNext
		f_DescriptorSet,                            // VkDescriptorSet                dstSet
		binding,                                    // uint32_t                       dstBinding
		0,                                          // uint32_t                       dstArrayElement
		buffer_infos.size(),                        // uint32_t                       descriptorCount
		VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,          // VkDescriptorType               descriptorType
		nullptr,                                    // const VkDescriptorImageInfo   *pImageInfo
		&buffer_infos[0],                           // const VkDescriptorBufferInfo  *pBufferInfo
		nullptr                                     // const VkBufferView            *pTexelBufferView
	};

	f_Device->f_DeviceFunctions.vkUpdateDescriptorSets(f_Device->f_Device, 1, &descriptor_writes, 0, nullptr);
}
//------------------------------------------------------------------------------
CM_END
