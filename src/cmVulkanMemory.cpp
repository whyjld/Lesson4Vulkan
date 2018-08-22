/**
	\brief Vulkan ÄÚ´æ·â×°Àà
*/
//------------------------------------------------------------------------------
#include "cmVulkanDevice.h"
#include "cmVulkanMemory.h"
//------------------------------------------------------------------------------
CM_BEGIN
//------------------------------------------------------------------------------
VulkanMemory::VulkanMemory()
	: f_Device(nullptr), f_Memory(VK_NULL_HANDLE), f_Size(0), f_Usage(new int(1))
{

}
//------------------------------------------------------------------------------
VulkanMemory::VulkanMemory(VkDeviceMemory memory, VkDeviceSize size, VulkanDevice* device)
	: f_Device(device), f_Memory(memory), f_Size(size), f_Usage(new int(1))
{
}
//------------------------------------------------------------------------------
VulkanMemory::VulkanMemory(const VulkanMemory& r)
	: f_Device(r.f_Device), f_Memory(r.f_Memory), f_Size(r.f_Size), f_Usage(r.f_Usage)
{
	++(*f_Usage);
}
//------------------------------------------------------------------------------
VulkanMemory::VulkanMemory(VulkanMemory&& r)
	: f_Device(r.f_Device), f_Memory(r.f_Memory), f_Size(r.f_Size), f_Usage(r.f_Usage)
{
	r.f_Device = nullptr;
	r.f_Memory = VK_NULL_HANDLE;
	r.f_Size = 0;
	r.f_Usage = new int(1);
}
//------------------------------------------------------------------------------
VulkanMemory::~VulkanMemory()
{
	if (0 == --(*f_Usage))
	{
		if (VK_NULL_HANDLE != f_Memory)
		{
			f_Device->f_DeviceFunctions.vkFreeMemory(f_Device->f_Device, f_Memory, nullptr);
		}
		delete f_Usage;
	}
}
//------------------------------------------------------------------------------
VulkanMemory& VulkanMemory::operator=(const VulkanMemory& r)
{
	if (this != &r)
	{
		if (0 == --(*f_Usage))
		{
			if (VK_NULL_HANDLE != f_Memory)
			{
				f_Device->f_DeviceFunctions.vkFreeMemory(f_Device->f_Device, f_Memory, nullptr);
			}
			delete f_Usage;
		}
		f_Device = r.f_Device;
		f_Memory = r.f_Memory;
		f_Size = r.f_Size;
		f_Usage = r.f_Usage;
		++(*f_Usage);
	}
	return *this;
}
//------------------------------------------------------------------------------
VulkanMemory& VulkanMemory::operator=(VulkanMemory&& r)
{
	if (this != &r)
	{
		std::swap(f_Device, r.f_Device);
		std::swap(f_Memory, r.f_Memory);
		std::swap(f_Size, r.f_Size);
		std::swap(f_Usage, r.f_Usage);
	}
	return *this;
}
//------------------------------------------------------------------------------
void* VulkanMemory::Map(VkDeviceSize offset, VkDeviceSize length)
{
	void *ret;
	if (f_Device->f_DeviceFunctions.vkMapMemory(f_Device->f_Device, f_Memory, offset, length, 0, &ret) != VK_SUCCESS)
	{
		THROW("Could not map memory and upload data to a vertex buffer!");
	}
	return ret;
}
//------------------------------------------------------------------------------
void VulkanMemory::Flush(VkDeviceSize offset, VkDeviceSize length)
{
	VkMappedMemoryRange flush_range =
	{
		VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE,            // VkStructureType        sType
		nullptr,                                          // const void            *pNext
		f_Memory,                                         // VkDeviceMemory         memory
		offset,                                           // VkDeviceSize           offset
		length                                            // VkDeviceSize           size
	};
	f_Device->f_DeviceFunctions.vkFlushMappedMemoryRanges(f_Device->f_Device, 1, &flush_range);
}
//------------------------------------------------------------------------------
void VulkanMemory::Unmap()
{
	f_Device->f_DeviceFunctions.vkUnmapMemory(f_Device->f_Device, f_Memory);
}
//------------------------------------------------------------------------------
CM_END
