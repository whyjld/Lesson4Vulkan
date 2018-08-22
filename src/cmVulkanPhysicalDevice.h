/**
	\brief Vulkan物理设备封装类
*/
//------------------------------------------------------------------------------
#ifndef _CM_VULKAN_PHYSICAL_DEVICE_h_
#define _CM_VULKAN_PHYSICAL_DEVICE_h_ "VulkanPhysicalDevice.h"
//------------------------------------------------------------------------------
#include "cmPublic.h"
#include "cmGLFWVulkan.h"
#include <vector>
//------------------------------------------------------------------------------
CM_BEGIN
class VulkanDevice;
class VulkanSurface;
//------------------------------------------------------------------------------
class VulkanPhysicalDevice
{
public:
	friend class VulkanInstance;

	VulkanPhysicalDevice(const VulkanPhysicalDevice& r);
	~VulkanPhysicalDevice();

	VkPhysicalDeviceProperties GetProperties() const;
	VkPhysicalDeviceFeatures   GetFeatures() const;
	VkPhysicalDeviceMemoryProperties GetMemoryProperties() const;
	std::vector<VkQueueFamilyProperties> GetQueueFamilyProperties() const;
	bool GetQueueSurfaceSupport(const VulkanSurface& surface, uint32_t queue) const;
	std::vector<VkExtensionProperties> GetExtensionProperties() const;
	VkSurfaceCapabilitiesKHR GetSurfaceCapabilities(const VulkanSurface& surface) const;
	std::vector<VkSurfaceFormatKHR> GetSurfaceFormats(const VulkanSurface& surface) const;
	std::vector<VkPresentModeKHR> GetSurfacePresentModes(const VulkanSurface& surface) const;

	/**
		\brief 创建逻辑设备
		\param queueFamilies 命令队列族
		\param queueTypes 命令队列类型，多个类型可能对应一个命令队列
		\param extensions 可选，创建设备需要支持的扩展
		\param surface 可选，设备显示的表面
		\return 逻辑设备对象
	*/
	VulkanDevice CreateDevice(const std::vector<QueueInfo>& queues, const std::vector<const char*> extensions = std::vector<const char*>(), const VulkanSurface* surface = nullptr);

	bool operator!() const
	{
		return VK_NULL_HANDLE == f_PhysicalDevice;
	}
private:
	VulkanPhysicalDevice() = delete;
	VulkanPhysicalDevice(VkPhysicalDevice physicalDevice);

	VkPhysicalDevice f_PhysicalDevice;
};
//------------------------------------------------------------------------------
CM_END
//------------------------------------------------------------------------------
#endif//_CM_VULKAN_PHYSICAL_DEVICE_h_