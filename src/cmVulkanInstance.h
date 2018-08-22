/**
	\brief Vulkan instance��װ��
*/
//------------------------------------------------------------------------------
#ifndef _CM_VULKAN_INSTANCE_h_
#define _CM_VULKAN_INSTANCE_h_ "cmVulkanInstance.h"
//------------------------------------------------------------------------------
#include "cmPublic.h"
#include "cmGLFWVulkan.h"
#include "cmVulkanDevice.h"
#include "cmVulkanSurface.h"
#include "cmVulkanPhysicalDevice.h"
#include <vector>
#include <map>
//------------------------------------------------------------------------------
CM_BEGIN
//------------------------------------------------------------------------------
class VulkanInstance
{
public:
	friend class VulkanFunctions;

	VulkanInstance();
	VulkanInstance(const char* application, const char* engine, const std::vector<const char*> extensions = std::vector<const char*>());
	VulkanInstance(VulkanInstance& r);
	~VulkanInstance();

	VulkanInstance& operator=(const VulkanInstance& r);
	VulkanInstance& operator=(VulkanInstance&& r);

	static std::vector<VkExtensionProperties> GetExtensionProperties();
	static bool CheckInstanceExtensionSupport(const std::vector<const char*>& extensions);
	static bool CheckInstanceExtensionSupport(const std::map<const char*, bool>& extensions);

	uint32_t GetPhysicalDeviceCount() const;
	VulkanPhysicalDevice& GetPhysicalDevice(uint32_t device);

	/**
		\brief �����߼��豸
		\param graphicsQueuePriorities ��Ⱦ�������ȼ��б����ȼ�����������Ⱦ��������
		\param presentQueuePriorities ��ʾ�������ȼ��б����ȼ�����������ʾ���������������п�����ʾ���к���Ⱦ������ͬ�������������Ⱦ��������������ʾ��������
		\param surface �豸��ʾ�ı���
		\param extensions ��ѡ�������豸��Ҫ֧�ֵ���չ
		\return �߼��豸����
	*/
	VulkanDevice CreateDevice(const std::vector<float>& graphicsQueuePriorities, const std::vector<float>& presentQueuePriorities, const VulkanSurface& surface, const std::vector<const char*> extensions = std::vector<const char*>());

#if defined(VK_USE_PLATFORM_WIN32_KHR)
	VulkanSurface CreateSurface(HINSTANCE instance, HWND hwnd);
#elif defined(VK_USE_PLATFORM_XCB_KHR)
	VulkanSurface CreateSurface(xcb_connection_t* connection, xcb_window_t window);
#elif defined(VK_USE_PLATFORM_XLIB_KHR)
	VulkanSurface CreateSurface(Display* dpy, Window handle);
#endif

	bool operator!() const
	{
		return VK_NULL_HANDLE == f_Instance;
	}
private:
	void InitInstanceLevelFunction();
	void EnumPhysicalDevices();

	static VulkanFunctions s_Functions;
	VkInstance f_Instance;
	std::vector<VulkanPhysicalDevice> f_PhysicaDevices;
	int* f_Usage;
};
//------------------------------------------------------------------------------
CM_END
//------------------------------------------------------------------------------
#endif//_CM_VULKAN_INSTANCE_h_