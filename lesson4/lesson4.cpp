// lesson4.cpp : ´´½¨½»»»Á´
//

#include "stdafx.h"
#include "../src/cmglfw.h"
#include "../src/cmvulkan.h"
#include <algorithm>
#include <map>

CM::GLFW g_GLFW(CM::apiVulken);
CM::GLFWwindow g_MainWindow;

CM::VulkanInstance g_Instance;

CM::VulkanDevice g_Device;
CM::VulkanQueue g_GraphicsQueue;
CM::VulkanQueue g_PresentQueue;

CM::VulkanSemaphore g_ImageAvailable;
CM::VulkanSemaphore g_RenderingFinished;

std::vector<CM::VulkanCommandBuffer> g_CommandBuffers;

CM::VulkanSurface g_Surface;
CM::VulkanSwapchain g_Swapchain;

bool InitializeInstance()
{
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
	if (!CM::VulkanInstance::CheckInstanceExtensionSupport(instanceExtensions))
	{
		return false;
	}
	g_Instance = CM::VulkanInstance("lesson4", "demo", instanceExtensions);
	return true;
}

void CreateDevice()
{
	g_MainWindow = g_GLFW.CreateWindow("lesson4", 640, 480);
	g_Surface = g_Instance.CreateSurface(nullptr, g_MainWindow.GetHWND());

	std::vector<const char*> deviceExtensions =
	{
		VK_KHR_SWAPCHAIN_EXTENSION_NAME,
	};

	std::vector<float> queuePriorities =
	{
		1.0f
	};

	g_Device = g_Instance.CreateDevice(queuePriorities, queuePriorities, g_Surface, deviceExtensions);
}

void CreateRendererSource()
{
	g_GraphicsQueue = g_Device.GetQueue(CM::qtGraphics, 0);
	g_PresentQueue = g_Device.GetQueue(CM::qtPresent, 0);
	g_ImageAvailable = g_Device.CreateSemaphore();
	g_RenderingFinished = g_Device.CreateSemaphore();

	g_CommandBuffers.push_back(g_Device.AllocCommandBuffer(CM::qtPresent));
	g_CommandBuffers.push_back(g_Device.AllocCommandBuffer(CM::qtPresent));
}

bool RecordCommandBuffers(CM::VulkanSwapchain& swapchain, CM::VulkanDevice& device, std::vector<CM::VulkanCommandBuffer>& buffers)
{
	VkClearColorValue clear_color =
	{
		{ 0.0f, 0.8f, 1.0f, 0.0f }
	};

	VkImageSubresourceRange image_subresource_range =
	{
		VK_IMAGE_ASPECT_COLOR_BIT,                    // VkImageAspectFlags                     aspectMask
		0,                                            // uint32_t                               baseMipLevel
		1,                                            // uint32_t                               levelCount
		0,                                            // uint32_t                               baseArrayLayer
		1                                             // uint32_t                               layerCount
	};

	uint32_t graphicsQueueFamily = g_GraphicsQueue.getFamily();
	uint32_t presentQueueFamily = g_PresentQueue.getFamily();

	auto images = swapchain.getImages();
	for (uint32_t i = 0; i < images.size(); ++i)
	{
		CM::VulkanCommandBuffer& buffer = buffers[i];

		buffer.BeginCommandBuffer(VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT);

		if (graphicsQueueFamily != presentQueueFamily)
		{
			VkImageMemoryBarrier barrier_from_present_to_clear =
			{
				VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,     // VkStructureType                        sType
				nullptr,                                    // const void                            *pNext
				VK_ACCESS_MEMORY_READ_BIT,                  // VkAccessFlags                          srcAccessMask
				VK_ACCESS_TRANSFER_WRITE_BIT,               // VkAccessFlags                          dstAccessMask
				VK_IMAGE_LAYOUT_UNDEFINED,                  // VkImageLayout                          oldLayout
				VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,       // VkImageLayout                          newLayout
				presentQueueFamily,                                // uint32_t                               srcQueueFamilyIndex
				graphicsQueueFamily,                                // uint32_t                               dstQueueFamilyIndex
				images[i],                                  // VkImage                                image
				image_subresource_range                     // VkImageSubresourceRange                subresourceRange
			};
			buffer.PipelineBarrier(VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0, nullptr, 1, &barrier_from_present_to_clear);
		}
		buffer.ClearColorImage(images[i], VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, &clear_color, 1, &image_subresource_range);

		if (graphicsQueueFamily != presentQueueFamily)
		{
			VkImageMemoryBarrier barrier_from_clear_to_present =
			{
				VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,     // VkStructureType                        sType
				nullptr,                                    // const void                            *pNext
				VK_ACCESS_TRANSFER_WRITE_BIT,               // VkAccessFlags                          srcAccessMask
				VK_ACCESS_MEMORY_READ_BIT,                  // VkAccessFlags                          dstAccessMask
				VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,       // VkImageLayout                          oldLayout
				VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,            // VkImageLayout                          newLayout
				graphicsQueueFamily,                                // uint32_t                               srcQueueFamilyIndex
				presentQueueFamily,                                // uint32_t                               dstQueueFamilyIndex
				images[i],                                  // VkImage                                image
				image_subresource_range                     // VkImageSubresourceRange                subresourceRange
			};
			buffer.PipelineBarrier(VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, 0, 0, nullptr, 0, nullptr, 1, &barrier_from_clear_to_present);
		}
		buffer.EndCommandBuffer();
	}

	return true;
}

void OnRefresh(CM::GLFWwindow* window)
{
	try
	{
		auto image = g_Swapchain.AcquireNextImage(g_ImageAvailable);
		g_GraphicsQueue.Submit(&g_ImageAvailable, VK_PIPELINE_STAGE_TRANSFER_BIT, g_CommandBuffers[image], &g_RenderingFinished);
		g_PresentQueue.Present(&g_RenderingFinished, &g_Swapchain, image);
	}
	catch (...)
	{
	}
}

void OnResize(CM::GLFWwindow* window, int width, int height)
{
	g_Swapchain = g_Device.CreateSwapchain(g_Surface, width, height, 2, &g_Swapchain);

	RecordCommandBuffers(g_Swapchain, g_Device, g_CommandBuffers);
}

int _tmain(int argc, _TCHAR* argv[])
{
	if (!InitializeInstance())
	{
		return 1;
	}

	CreateDevice();
	CreateRendererSource();

	g_MainWindow.OnRefresh = OnRefresh;
	g_MainWindow.OnResize = OnResize;
	g_MainWindow.Show();

	for (; !g_MainWindow.ShouldClose();)
	{
		OnRefresh(&g_MainWindow);
		g_GLFW.WaitEvents();
	}

	return 0;
}

