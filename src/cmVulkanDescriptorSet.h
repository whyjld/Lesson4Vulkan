/**
	\brief VulkanÃèÊö·û¼¯·â×°Àà
*/
//------------------------------------------------------------------------------
#ifndef _CM_VULKAN_DESCRIPTOR_SET_h_
#define _CM_VULKAN_DESCRIPTOR_SET_h_ "cmVulkanDescriptorSet.h"
//------------------------------------------------------------------------------
#include "cmPublic.h"
#include "cmGLFWVulkan.h"
#include <vector>
//------------------------------------------------------------------------------
CM_BEGIN
//------------------------------------------------------------------------------
class VulkanDevice;
//------------------------------------------------------------------------------
class VulkanDescriptorSet
{
public:
	friend class VulkanDevice;
	friend class VulkanSampler;
	friend class VulkanImageView;
	friend class VulkanCommandBuffer;
	friend class VulkanDescriptorSetLayout;

	VulkanDescriptorSet();
	VulkanDescriptorSet(VulkanDescriptorSet& r);
	~VulkanDescriptorSet();

	VulkanDescriptorSet& operator=(const VulkanDescriptorSet& r);
	VulkanDescriptorSet& operator=(VulkanDescriptorSet&& r);

	struct CombinedImageSampler
	{
		VulkanImageView* View;
		VulkanSampler* Sampler;
	};

	struct BufferInfo
	{
		VulkanBuffer* Buffer;
		VkDeviceSize Offset;
		VkDeviceSize Range;
	};

	void Update(uint32_t binding, const std::vector<CombinedImageSampler>& combinedImageSamplers);
	void Update(uint32_t binding, const std::vector<BufferInfo>& buffers);

	bool operator!() const
	{
		return VK_NULL_HANDLE == f_DescriptorSet;
	}
private:
	VulkanDescriptorSet(VkDescriptorSet ds, VulkanDevice* device);

	VulkanDevice* f_Device;
	VkDescriptorSet f_DescriptorSet;

	int* f_Usage;
};
//------------------------------------------------------------------------------
CM_END
//------------------------------------------------------------------------------
#endif//_CM_VULKAN_DESCRIPTOR_SET_LAYOUT_h_