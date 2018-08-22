/**
	\brief VulkanÄÚ´æ·â×°Àà
*/
//------------------------------------------------------------------------------
#ifndef _CM_VULKAN_MEMORY_h_
#define _CM_VULKAN_MEMORY_h_ "cmVulkanMemory.h"
//------------------------------------------------------------------------------
#include "cmPublic.h"
#include "cmGLFWVulkan.h"
#include <vector>
//------------------------------------------------------------------------------
CM_BEGIN
//------------------------------------------------------------------------------
class VulkanDevice;
//------------------------------------------------------------------------------
class VulkanMemory
{
public:
	friend class VulkanDevice;
	friend class VulkanBuffer;
	friend class VulkanImage;

	VulkanMemory();
	VulkanMemory(const VulkanMemory& r);
	VulkanMemory(VulkanMemory&& r);
	~VulkanMemory();

	VulkanMemory& operator=(const VulkanMemory& r);
	VulkanMemory& operator=(VulkanMemory&& r);

	void* Map(VkDeviceSize offset, VkDeviceSize length);
	void Flush(VkDeviceSize offset, VkDeviceSize length);
	void Unmap();

	VkDeviceSize getSize() const
	{
		return f_Size;
	}
		
	bool operator!() const
	{
		return VK_NULL_HANDLE == f_Memory;
	}
private:
	VulkanMemory(VkDeviceMemory memory, VkDeviceSize size, VulkanDevice* device);

	VulkanDevice* f_Device;
	VkDeviceMemory f_Memory;
	VkDeviceSize f_Size;

	int* f_Usage;
};
//------------------------------------------------------------------------------
CM_END
//------------------------------------------------------------------------------
#endif//_CM_VULKAN_MEMORY_h_