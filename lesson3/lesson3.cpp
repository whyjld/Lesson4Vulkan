// lesson3.cpp : 创建逻辑设备。
//

#include "stdafx.h"
#include "../src/cmglfw.h"
#include "../src/cmvulkan.h"
#include <algorithm>
#include <map>

int _tmain(int argc, _TCHAR* argv[])
{
	CM::GLFW glfw(CM::apiVulken);
	CM::GLFWwindow main = glfw.CreateWindow("Title", 640, 480);

	std::vector<const char*> instanceExtensions =
	{
		VK_KHR_SURFACE_EXTENSION_NAME,
#if defined(VK_USE_PLATFORM_WIN32_KHR)
		VK_KHR_WIN32_SURFACE_EXTENSION_NAME,
#elif defined(VK_USE_PLATFORM_XCB_KHR)
		VK_KHR_XCB_SURFACE_EXTENSION_NAME,
#elif defined(VK_USE_PLATFORM_XLIB_KHR)
		VK_KHR_XLIB_SURFACE_EXTENSION_NAME,
#endif
	};
	CM::VulkanInstance vi("lesson3", "demo", instanceExtensions);
	CM::VulkanSurface surface = vi.CreateSurface(nullptr, main.GetHWND());

	std::vector<const char*> deviceExtensions =
	{
		VK_KHR_SWAPCHAIN_EXTENSION_NAME,
	};

	std::vector<float> graphicsQueuePriorities =
	{
		1.0f,
		1.0f,
	};

	std::vector<float> swapchainQueuePriorities =
	{
		1.0f
	};

	CM::VulkanDevice device = vi.CreateDevice(graphicsQueuePriorities, swapchainQueuePriorities, surface, deviceExtensions);;

	return 0;
}

