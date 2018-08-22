/**
	\brief Vulkan »º³å·â×°Àà
*/
//------------------------------------------------------------------------------
#include "cmVulkanDevice.h"
#include "cmVulkanBuffer.h"
//------------------------------------------------------------------------------
CM_BEGIN
//------------------------------------------------------------------------------
VulkanBuffer::VulkanBuffer()
	: f_Device(nullptr), f_Buffer(VK_NULL_HANDLE), f_Size(0), f_Offset(~VkDeviceSize(0)), f_Usage(new int(1))
{

}
//------------------------------------------------------------------------------
VulkanBuffer::VulkanBuffer(VkBuffer buffer, VulkanDevice* device)
	: f_Device(device), f_Buffer(buffer), f_Offset(~VkDeviceSize(0)), f_Usage(new int(1))
{
	VkMemoryRequirements buffer_memory_requirements;
	f_Device->f_DeviceFunctions.vkGetBufferMemoryRequirements(f_Device->f_Device, f_Buffer, &buffer_memory_requirements);
	f_Size = buffer_memory_requirements.size;
}
//------------------------------------------------------------------------------
VulkanBuffer::VulkanBuffer(const VulkanBuffer& r)
	: f_Device(r.f_Device), f_Buffer(r.f_Buffer), f_Size(r.f_Size), f_Memory(r.f_Memory), f_Offset(r.f_Offset), f_Usage(r.f_Usage)
{
	++(*f_Usage);
}
//------------------------------------------------------------------------------
VulkanBuffer::~VulkanBuffer()
{
	if (0 == --(*f_Usage))
	{
		if (VK_NULL_HANDLE != f_Buffer)
		{
			f_Device->f_DeviceFunctions.vkDestroyBuffer(f_Device->f_Device, f_Buffer, nullptr);
		}
		delete f_Usage;
	}
}
//------------------------------------------------------------------------------
VulkanBuffer& VulkanBuffer::operator=(const VulkanBuffer& r)
{
	if (this != &r)
	{
		if (0 == --(*f_Usage))
		{
			if (VK_NULL_HANDLE != f_Buffer)
			{
				f_Device->f_DeviceFunctions.vkDestroyBuffer(f_Device->f_Device, f_Buffer, nullptr);
			}
			delete f_Usage;
		}
		f_Device = r.f_Device;
		f_Buffer = r.f_Buffer;
		f_Size = r.f_Size;
		f_Memory = r.f_Memory;
		f_Offset = r.f_Offset;
		f_Usage = r.f_Usage;
		++(*f_Usage);
	}
	return *this;
}
//------------------------------------------------------------------------------
VulkanBuffer& VulkanBuffer::operator=(VulkanBuffer&& r)
{
	if (this != &r)
	{
		std::swap(f_Device, r.f_Device);
		std::swap(f_Buffer, r.f_Buffer);
		std::swap(f_Size, r.f_Size);
		std::swap(f_Memory, r.f_Memory);
		std::swap(f_Offset, r.f_Offset);
		std::swap(f_Usage, r.f_Usage);
	}
	return *this;
}
//------------------------------------------------------------------------------
VkBufferMemoryBarrier VulkanBuffer::CreateBufferMemoryBarrier(VkAccessFlags srcAccessMask,
	VkAccessFlags dstAccessMask,
	uint32_t srcQueueFamilyIndex,
	uint32_t dstQueueFamilyIndex,
	VkDeviceSize offset,
	VkDeviceSize size)
{
	return VkBufferMemoryBarrier(
	{
		VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER, // VkStructureType                        sType
		nullptr,                                 // const void                            *pNext
		srcAccessMask,                           // VkAccessFlags                          srcAccessMask
		dstAccessMask,                           // VkAccessFlags                          dstAccessMask
		srcQueueFamilyIndex,                     // uint32_t                               srcQueueFamilyIndex
		dstQueueFamilyIndex,                     // uint32_t                               dstQueueFamilyIndex
		f_Buffer,                                // VkBuffer                               buffer
		offset,                                  // VkDeviceSize                           offset
		size                                     // VkDeviceSize                           size
	});
}
//------------------------------------------------------------------------------
VkDeviceSize VulkanBuffer::Bind(const VulkanMemory& memory, VkDeviceSize offset)
{
	VkMemoryRequirements buffer_memory_requirements;
	f_Device->f_DeviceFunctions.vkGetBufferMemoryRequirements(f_Device->f_Device, f_Buffer, &buffer_memory_requirements);
	f_Size = buffer_memory_requirements.size;

	offset += buffer_memory_requirements.alignment - 1;
	offset /= buffer_memory_requirements.alignment;
	offset *= buffer_memory_requirements.alignment;

	if (offset + buffer_memory_requirements.size > memory.getSize())
	{
		THROW("No enough memory.");
	}

	if (innerBind(memory, offset))
	{
		THROW("Could not bind buffer to memory.");
	}
	return offset;
}
//------------------------------------------------------------------------------
bool VulkanBuffer::innerBind(const VulkanMemory& memory, VkDeviceSize offset)
{
	if (f_Device->f_DeviceFunctions.vkBindBufferMemory(f_Device->f_Device, f_Buffer, memory.f_Memory, offset) == VK_SUCCESS)
	{
		f_Memory = memory;
		f_Offset = offset;
		return true;
	}
	return false;
}
//------------------------------------------------------------------------------
CM_END
