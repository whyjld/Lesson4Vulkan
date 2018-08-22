/**
	\brief Vulkan instance·â×°Àà
*/
//------------------------------------------------------------------------------
#include "cmVulkanInstance.h"
#include "cmUtility.h"
#include <algorithm>
//------------------------------------------------------------------------------
CM_BEGIN
//------------------------------------------------------------------------------
class VulkanFunctions
{
public:
	VulkanFunctions()
		: f_Initialized(VK_SUCCESS == volkInitialize())
	{
	}
	~VulkanFunctions()
	{

	}
private:
	bool f_Initialized;
};
//------------------------------------------------------------------------------
VulkanFunctions VulkanInstance::s_Functions;
//------------------------------------------------------------------------------
VulkanInstance::VulkanInstance()
	: f_Instance(VK_NULL_HANDLE), f_Usage(new int(1))
{
}
//------------------------------------------------------------------------------
VulkanInstance::VulkanInstance(const char* application, const char* engine, const std::vector<const char*> extensions)
	: VulkanInstance()
{
	VkApplicationInfo application_info =
	{
		VK_STRUCTURE_TYPE_APPLICATION_INFO, // VkStructureType            sType
		nullptr,                            // const void                *pNext
		application,                        // const char                *pApplicationName
		VK_MAKE_VERSION(1, 0, 0),           // uint32_t                   applicationVersion
		engine,                             // const char                *pEngineName
		VK_MAKE_VERSION(1, 0, 0),           // uint32_t                   engineVersion
		VK_MAKE_VERSION(1, 0, 0)            // uint32_t                   apiVersion
	};

	VkInstanceCreateInfo instance_create_info =
	{
		VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,         // VkStructureType            sType
		nullptr,                                        // const void*                pNext
		0,                                              // VkInstanceCreateFlags      flags
		&application_info,                              // const VkApplicationInfo   *pApplicationInfo
		0,                                              // uint32_t                   enabledLayerCount
		nullptr,                                        // const char * const        *ppEnabledLayerNames
		extensions.size(),                               // uint32_t                   enabledExtensionCount
		extensions.size() > 0 ? &extensions[0] : nullptr // const char * const        *ppEnabledExtensionNames
	};

	if (vkCreateInstance(&instance_create_info, nullptr, &f_Instance) != VK_SUCCESS)
	{
		THROW("Could not create Vulkan instance!");
	}
	volkLoadInstance(f_Instance);
	EnumPhysicalDevices();
}
//------------------------------------------------------------------------------
VulkanInstance::VulkanInstance(VulkanInstance& r)
	: f_Instance(VK_NULL_HANDLE), f_PhysicaDevices(r.f_PhysicaDevices), f_Usage(r.f_Usage)
{
	++(*f_Usage);
}
//------------------------------------------------------------------------------
VulkanInstance::~VulkanInstance()
{
	if (0 == --(*f_Usage))
	{
		f_PhysicaDevices.clear();
		if (VK_NULL_HANDLE != f_Instance)
		{
			vkDestroyInstance(f_Instance, nullptr);
		}
	}
}
//------------------------------------------------------------------------------
VulkanInstance& VulkanInstance::operator=(const VulkanInstance& r)
{
	if (&r != this)
	{
		if (0 == --(*f_Usage))
		{
			f_PhysicaDevices.clear();
			if (VK_NULL_HANDLE != f_Instance)
			{
				vkDestroyInstance(f_Instance, nullptr);
			}
		}
		f_Instance = r.f_Instance;
		f_PhysicaDevices = r.f_PhysicaDevices;
		f_Usage = r.f_Usage;
		++(*f_Usage);
	}
	return *this;
}
//------------------------------------------------------------------------------
VulkanInstance& VulkanInstance::operator=(VulkanInstance&& r)
{
	if (&r != this)
	{
		std::swap(f_Instance, r.f_Instance);
		std::swap(f_PhysicaDevices, r.f_PhysicaDevices);
		std::swap(f_Usage, r.f_Usage);
	}
	return *this;
}
//------------------------------------------------------------------------------
std::vector<VkExtensionProperties> VulkanInstance::GetExtensionProperties()
{
	uint32_t extensionsCount = 0;
	if ((vkEnumerateInstanceExtensionProperties(nullptr, &extensionsCount, nullptr) != VK_SUCCESS)
		|| (0 == extensionsCount))
	{
		THROW("Error occurred during instance extensions enumeration!");
	}
	
	std::vector<VkExtensionProperties> ret(extensionsCount);
	if (vkEnumerateInstanceExtensionProperties(nullptr, &extensionsCount, &ret[0]) != VK_SUCCESS)
	{
		THROW("Error occurred during instance extensions enumeration!");
	}
	return ret;
}
//------------------------------------------------------------------------------
bool VulkanInstance::CheckInstanceExtensionSupport(const std::vector<const char*>& extensions)
{
	return CM::CheckExtensionSupport(GetExtensionProperties(), extensions);
}
//------------------------------------------------------------------------------
bool VulkanInstance::CheckInstanceExtensionSupport(const std::map<const char*, bool>& extensions)
{
	auto exts = VulkanInstance::GetExtensionProperties();

	bool ret = true;
	for (auto e : extensions)
	{
		e.second = std::any_of(begin(exts), end(exts), [e](const VkExtensionProperties& ep)->bool{return 0 == strcmp(e.first, ep.extensionName); });
		ret &= e.second;
	}
	return ret;
}
//------------------------------------------------------------------------------
uint32_t VulkanInstance::GetPhysicalDeviceCount() const
{
	return f_PhysicaDevices.size();
}
//------------------------------------------------------------------------------
VulkanPhysicalDevice& VulkanInstance::GetPhysicalDevice(uint32_t device)
{
	if (device < f_PhysicaDevices.size())
	{
		return f_PhysicaDevices[device];
	}
	THROW("Out of device count.");
}
//------------------------------------------------------------------------------
VulkanDevice VulkanInstance::CreateDevice(const std::vector<float>& graphicsQueuePriorities, const std::vector<float>& presentQueuePriorities, const VulkanSurface& surface, const std::vector<const char*> extensions)
{
	auto FindGraphicsQueue = [](const std::vector<VkQueueFamilyProperties>& qfps)->uint32_t
	{
		for (uint32_t qf = 0; qf < qfps.size(); ++qf)
		{
			auto qfp = qfps[qf];
			if (qfp.queueCount > 0 && 0 != (qfp.queueFlags & VK_QUEUE_GRAPHICS_BIT))
			{
				return qf;
			}
		}
		return (uint32_t)-1;
	};

	auto FindSwapChainQueue = [&surface](const CM::VulkanPhysicalDevice& pd, const std::vector<VkQueueFamilyProperties>& qfps)->uint32_t
	{
		for (uint32_t qf = 0; qf < qfps.size(); ++qf)
		{
			if (pd.GetQueueSurfaceSupport(surface, qf))
			{
				return qf;
			}
		}
		return (uint32_t)-1;
	};

	uint32_t pdc = GetPhysicalDeviceCount();
	for (uint32_t i = 0; i < pdc; ++i)
	{
		VulkanPhysicalDevice& pd = GetPhysicalDevice(i);
		if (!CM::CheckExtensionSupport(pd.GetExtensionProperties(), extensions))
		{
			continue;
		}

		auto qfp = pd.GetQueueFamilyProperties();

		uint32_t graphicsQueue = FindGraphicsQueue(qfp);
		if (((uint32_t)-1) == graphicsQueue)
		{
			continue;
		}

		uint32_t presentQueue = graphicsQueue;
		if (!pd.GetQueueSurfaceSupport(surface, presentQueue))
		{
			presentQueue = FindSwapChainQueue(pd, qfp);
			if (((uint32_t)-1) == presentQueue)
			{
				continue;
			}
		}

		std::vector<QueueInfo> queues;

		QueueInfo graphicsQueueInfo =
		{
			graphicsQueue,
			graphicsQueuePriorities,
			{ qtGraphics },
		};
		if (presentQueue == graphicsQueue)
		{
			graphicsQueueInfo.Types.push_back(qtPresent);
		}
		else
		{
			queues.push_back(QueueInfo(
			{
				presentQueue,
				presentQueuePriorities,
				{ qtPresent },
			}));
		}
		queues.push_back(graphicsQueueInfo);

		return pd.CreateDevice(queues, extensions, &surface);
	}
	THROW("Can't select device.");
	return VulkanDevice();
}
//------------------------------------------------------------------------------
#if defined(VK_USE_PLATFORM_WIN32_KHR)
VulkanSurface VulkanInstance::CreateSurface(HINSTANCE instance, HWND hwnd)
#elif defined(VK_USE_PLATFORM_XCB_KHR)
VulkanSurface VulkanInstance::CreateSurface(xcb_connection_t* connection, xcb_window_t window)
#elif defined(VK_USE_PLATFORM_XLIB_KHR)
VulkanSurface VulkanInstance::CreateSurface(Display* dpy, Window handle)
#endif
{
	VkSurfaceKHR surface = 0;
#if defined(VK_USE_PLATFORM_WIN32_KHR)
	VkWin32SurfaceCreateInfoKHR surfaceCreateInfo =
	{
		VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR,  // VkStructureType                  sType
		nullptr,                                          // const void                      *pNext
		0,                                                // VkWin32SurfaceCreateFlagsKHR     flags
		instance,                                         // HINSTANCE                        hinstance
		hwnd                                              // HWND                             hwnd
	};

	if (vkCreateWin32SurfaceKHR(f_Instance, &surfaceCreateInfo, nullptr, &surface) != VK_SUCCESS)
	{
		THROW("Create win32 surface failed.");
	}

#elif defined(VK_USE_PLATFORM_XCB_KHR)
	VkXcbSurfaceCreateInfoKHR surfaceCreateInfo = {
		VK_STRUCTURE_TYPE_XCB_SURFACE_CREATE_INFO_KHR,    // VkStructureType                  sType
		nullptr,                                          // const void                      *pNext
		0,                                                // VkXcbSurfaceCreateFlagsKHR       flags
		connection,                                       // xcb_connection_t*                connection
		window                                            // xcb_window_t                     window
	};

	if (vkCreateXcbSurfaceKHR(f_Instance, &surfaceCreateInfo, nullptr, &surface) != VK_SUCCESS)
	{
		THROW("Create xcb surface failed.");
	}

#elif defined(VK_USE_PLATFORM_XLIB_KHR)
	VkXlibSurfaceCreateInfoKHR surfaceCreateInfo = {
		VK_STRUCTURE_TYPE_XLIB_SURFACE_CREATE_INFO_KHR,   // VkStructureType                sType
		nullptr,                                          // const void                    *pNext
		0,                                                // VkXlibSurfaceCreateFlagsKHR    flags
		dpy,                                              // Display                       *dpy
		handle                                            // Window                         window
	};
	if (vkCreateXlibSurfaceKHR(f_Instance, &surfaceCreateInfo, nullptr, &surface) != VK_SUCCESS)
	{
		THROW("Create xlib surface failed.");
	}

#endif
	return VulkanSurface(f_Instance, surface);
}
//------------------------------------------------------------------------------
void VulkanInstance::EnumPhysicalDevices()
{
	uint32_t deviceCount = 0;
	if ((vkEnumeratePhysicalDevices(f_Instance, &deviceCount, nullptr) != VK_SUCCESS)
		|| (0 == deviceCount))
	{
		THROW("Error occurred during physical devices enumeration!");
	}

	std::vector<VkPhysicalDevice> physicalDevices;
	physicalDevices.resize(deviceCount);
	if (vkEnumeratePhysicalDevices(f_Instance, &deviceCount, &physicalDevices[0]) != VK_SUCCESS)
	{
		THROW("Error occurred during physical devices enumeration!");
	}

	for (auto pd : physicalDevices)
	{
		f_PhysicaDevices.push_back(VulkanPhysicalDevice(pd));
	}
}
//------------------------------------------------------------------------------
CM_END
