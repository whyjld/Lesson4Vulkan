/**
	\brief Vulkan�����豸��װ��
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
		\brief �����߼��豸
		\param queueFamilies ���������
		\param queueTypes ����������ͣ�������Ϳ��ܶ�Ӧһ���������
		\param extensions ��ѡ�������豸��Ҫ֧�ֵ���չ
		\param surface ��ѡ���豸��ʾ�ı���
		\return �߼��豸����
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