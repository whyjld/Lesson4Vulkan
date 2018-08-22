/**
	\brief Vulkan物理设备封装类
*/
//------------------------------------------------------------------------------
#include "cmUtility.h"
#include "cmVulkanInstance.h"
#include "cmVulkanPhysicalDevice.h"
#include <set>
//------------------------------------------------------------------------------
CM_BEGIN
//------------------------------------------------------------------------------
VulkanPhysicalDevice::VulkanPhysicalDevice(VkPhysicalDevice physicalDevice)
	: f_PhysicalDevice(physicalDevice)
{
}
//------------------------------------------------------------------------------
VulkanPhysicalDevice::VulkanPhysicalDevice(const VulkanPhysicalDevice& r)
	: f_PhysicalDevice(r.f_PhysicalDevice)
{
}
//------------------------------------------------------------------------------
VulkanPhysicalDevice::~VulkanPhysicalDevice()
{
}
//------------------------------------------------------------------------------
VkPhysicalDeviceProperties VulkanPhysicalDevice::GetProperties() const
{
	VkPhysicalDeviceProperties ret;
	vkGetPhysicalDeviceProperties(f_PhysicalDevice, &ret);
	return ret;
}
//------------------------------------------------------------------------------
VkPhysicalDeviceFeatures VulkanPhysicalDevice::GetFeatures() const
{
	VkPhysicalDeviceFeatures ret;
	vkGetPhysicalDeviceFeatures(f_PhysicalDevice, &ret);
	return ret;
}
//------------------------------------------------------------------------------
VkPhysicalDeviceMemoryProperties VulkanPhysicalDevice::GetMemoryProperties() const
{
	VkPhysicalDeviceMemoryProperties ret;
	vkGetPhysicalDeviceMemoryProperties(f_PhysicalDevice, &ret);
	return ret;
}
//------------------------------------------------------------------------------
std::vector<VkQueueFamilyProperties> VulkanPhysicalDevice::GetQueueFamilyProperties() const
{
	uint32_t queueFamiliesCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(f_PhysicalDevice, &queueFamiliesCount, nullptr);
	if (0 == queueFamiliesCount)
	{
		THROW("Physical device doesn't have any queue families!");
	}

	std::vector<VkQueueFamilyProperties> ret(queueFamiliesCount);
	vkGetPhysicalDeviceQueueFamilyProperties(f_PhysicalDevice, &queueFamiliesCount, &ret[0]);
	return ret;
}
//------------------------------------------------------------------------------
bool VulkanPhysicalDevice::GetQueueSurfaceSupport(const VulkanSurface& surface, uint32_t queue) const
{
	VkBool32 ret;
	vkGetPhysicalDeviceSurfaceSupportKHR(f_PhysicalDevice, queue, surface.f_Surface, &ret);
	return (ret != 0);
}
//------------------------------------------------------------------------------
std::vector<VkExtensionProperties> VulkanPhysicalDevice::GetExtensionProperties() const
{
	uint32_t extensionsCount = 0;
	if ((vkEnumerateDeviceExtensionProperties(f_PhysicalDevice, nullptr, &extensionsCount, nullptr) != VK_SUCCESS)
		|| (0 == extensionsCount))
	{
		THROW("Error occurred during physical device extensions enumeration!");
	}

	std::vector<VkExtensionProperties> ret(extensionsCount);
	if (vkEnumerateDeviceExtensionProperties(f_PhysicalDevice, nullptr, &extensionsCount, &ret[0]) != VK_SUCCESS)
	{
		THROW("Error occurred during physical device extensions enumeration!");
	}
	return ret;
}
//------------------------------------------------------------------------------
VkSurfaceCapabilitiesKHR VulkanPhysicalDevice::GetSurfaceCapabilities(const VulkanSurface& surface) const
{
	VkSurfaceCapabilitiesKHR ret;
	if (vkGetPhysicalDeviceSurfaceCapabilitiesKHR(f_PhysicalDevice, surface.f_Surface, &ret) != VK_SUCCESS)
	{
		THROW("Could not check presentation surface capabilities!");
	}
	return ret;
}
//------------------------------------------------------------------------------
std::vector<VkSurfaceFormatKHR> VulkanPhysicalDevice::GetSurfaceFormats(const VulkanSurface& surface) const
{
	uint32_t formatsCount;
	if ((vkGetPhysicalDeviceSurfaceFormatsKHR(f_PhysicalDevice, surface.f_Surface, &formatsCount, nullptr) != VK_SUCCESS) || (0 == formatsCount))
	{
		THROW("Error occurred during presentation surface formats enumeration!");
	}

	std::vector<VkSurfaceFormatKHR> ret(formatsCount);
	if (vkGetPhysicalDeviceSurfaceFormatsKHR(f_PhysicalDevice, surface.f_Surface, &formatsCount, &ret[0]) != VK_SUCCESS)
	{
		THROW("Error occurred during presentation surface formats enumeration!");
	}
	return ret;
}
//------------------------------------------------------------------------------
std::vector<VkPresentModeKHR> VulkanPhysicalDevice::GetSurfacePresentModes(const VulkanSurface& surface) const
{
	uint32_t presentModesCount;
	if ((vkGetPhysicalDeviceSurfacePresentModesKHR(f_PhysicalDevice, surface.f_Surface, &presentModesCount, nullptr) != VK_SUCCESS) || (0 == presentModesCount))
	{
		THROW("Error occurred during presentation surface present modes enumeration!");
	}

	std::vector<VkPresentModeKHR> presentModes(presentModesCount);
	if (vkGetPhysicalDeviceSurfacePresentModesKHR(f_PhysicalDevice, surface.f_Surface, &presentModesCount, &presentModes[0]) != VK_SUCCESS)
	{
		THROW("Error occurred during presentation surface present modes enumeration!");
	}
	return presentModes;
}
//------------------------------------------------------------------------------
VulkanDevice VulkanPhysicalDevice::CreateDevice(const std::vector<QueueInfo>& queues, const std::vector<const char*> extensions, const VulkanSurface* surface)
{
	std::set<QueueType> queueTypes;
	for (auto q : queues)
	{
		for (auto t : q.Types)
		{
			if (queueTypes.find(t) == queueTypes.end())
			{
				queueTypes.insert(t);
			}
			else
			{
				THROW("More than one queue have same type!");
			}
		}
	}

	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
	for (auto q : queues)
	{
		queueCreateInfos.push_back(VkDeviceQueueCreateInfo({
			VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
			nullptr,
			0,
			q.Family,
			q.Priorities.size(),
			q.Priorities.data(),
		}));
	}

	VkPhysicalDeviceFeatures features = GetFeatures();

	VkDeviceCreateInfo deviceCreateInfo =
	{
		VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,           // VkStructureType                    sType
		nullptr,                                        // const void                        *pNext
		0,                                              // VkDeviceCreateFlags                flags
		queueCreateInfos.size(),                        // uint32_t                           queueCreateInfoCount
		&queueCreateInfos[0],                           // const VkDeviceQueueCreateInfo     *pQueueCreateInfos
		0,                                              // uint32_t                           enabledLayerCount
		nullptr,                                        // const char * const                *ppEnabledLayerNames
		extensions.size(),                              // uint32_t                           enabledExtensionCount
		extensions.size() > 0 ? &extensions[0] : nullptr, // const char * const                *ppEnabledExtensionNames
		&features                                       // const VkPhysicalDeviceFeatures    *pEnabledFeatures
	};

	VkDevice deviceHandle = nullptr;
	if (vkCreateDevice(f_PhysicalDevice, &deviceCreateInfo, nullptr, &deviceHandle) != VK_SUCCESS)
	{
		THROW("Could not create Vulkan device!");
		return VulkanDevice();
	}
	return VulkanDevice(deviceHandle, queues, *this);
}
//------------------------------------------------------------------------------
CM_END
