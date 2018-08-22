// lesson5.cpp : 绘制三角形
// 这个demo中使用shader获得顶点坐标
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
CM::VulkanPipelineLayout g_PipelineLayout;

CM::VulkanSemaphore g_ImageAvailable;
CM::VulkanSemaphore g_RenderingFinished;

std::vector<CM::VulkanCommandBuffer> g_CommandBuffers;

CM::VulkanShader g_VertexShader;
CM::VulkanShader g_FragmentShader;

std::vector<CM::ShaderStageInfo> g_Shaders;

CM::VulkanSurface g_Surface;
CM::VulkanSwapchain g_Swapchain;
CM::VulkanRenderPass g_RenderPass;
CM::VulkanGraphicsPipeline g_Pipeline;
std::vector<CM::VulkanFramebuffer> g_Framebuffers;

const char* title = "lesson5";

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
	g_Instance = CM::VulkanInstance(title, "demo", instanceExtensions);
	return true;
}

void CreateDevice()
{
	g_MainWindow = g_GLFW.CreateWindow(title, 640, 480);
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
	g_PipelineLayout = g_Device.CreatePipelineLayout(std::vector<CM::VulkanDescriptorSetLayout>());
	g_ImageAvailable = g_Device.CreateSemaphore();
	g_RenderingFinished = g_Device.CreateSemaphore();
	g_CommandBuffers.push_back(g_Device.AllocCommandBuffer(CM::qtPresent));
	g_CommandBuffers.push_back(g_Device.AllocCommandBuffer(CM::qtPresent));

	g_VertexShader = g_Device.CreateShader("Data/shader.vert.spv");
	g_FragmentShader = g_Device.CreateShader("Data/shader.frag.spv");

	g_Shaders.push_back(CM::ShaderStageInfo({ &g_VertexShader, VK_SHADER_STAGE_VERTEX_BIT, "main" }));
	g_Shaders.push_back(CM::ShaderStageInfo({ &g_FragmentShader, VK_SHADER_STAGE_FRAGMENT_BIT, "main" }));
}

bool RecordCommandBuffers(CM::VulkanSwapchain& swapchain, CM::VulkanDevice& device, std::vector<CM::VulkanCommandBuffer>& buffers)
{
	VkImageSubresourceRange image_subresource_range =
	{
		VK_IMAGE_ASPECT_COLOR_BIT,                      // VkImageAspectFlags             aspectMask
		0,                                              // uint32_t                       baseMipLevel
		1,                                              // uint32_t                       levelCount
		0,                                              // uint32_t                       baseArrayLayer
		1                                               // uint32_t                       layerCount
	};

	VkClearValue clear_value =
	{
		{ 0.0f, 0.8f, 0.4f, 0.0f },                     // VkClearColorValue              color
	};

	auto images = g_Swapchain.getImages();

	uint32_t graphicsQueueFamily = g_GraphicsQueue.getFamily();
	uint32_t presentQueueFamily = g_PresentQueue.getFamily();

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
				presentQueueFamily,                         // uint32_t                               srcQueueFamilyIndex
				graphicsQueueFamily,                        // uint32_t                               dstQueueFamilyIndex
				images[i],                                  // VkImage                                image
				image_subresource_range                     // VkImageSubresourceRange                subresourceRange
			};
			buffer.PipelineBarrier(VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0, nullptr, 1, &barrier_from_present_to_clear);
		}

		buffer.BeginRenderPass(g_RenderPass, g_Framebuffers[i], VkRect2D({ { 0, 0 }, swapchain.getSize() }), clear_value);
		buffer.BindPipeline(g_Pipeline);
		buffer.Draw(3, 1, 0, 0);

		buffer.EndRenderPass();

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
				graphicsQueueFamily,                        // uint32_t                               srcQueueFamilyIndex
				presentQueueFamily,                         // uint32_t                               dstQueueFamilyIndex
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
		g_GraphicsQueue.Submit(&g_ImageAvailable, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, g_CommandBuffers[image], &g_RenderingFinished);
		g_PresentQueue.Present(&g_RenderingFinished, &g_Swapchain, image);
	}
	catch (...)
	{
	}
}

void OnResize(CM::GLFWwindow* window, int width, int height)
{
	g_Swapchain = g_Device.CreateSwapchain(g_Surface, 0, 0, 2, &g_Swapchain);
	g_RenderPass = g_Device.CreateRenderPass(g_Swapchain.getFormat());
	g_Pipeline = g_Device.CreateGraphicsPipeline(g_RenderPass, g_PipelineLayout, g_Shaders, VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, VkRect2D({ { 0, 0 }, g_Swapchain.getSize() }));

	g_Framebuffers.resize(g_Swapchain.getImageCount());
	for (size_t i = 0; i < g_Swapchain.getImageCount(); ++i)
	{
		g_Framebuffers[i] = g_Device.CreateFramebuffer(g_Swapchain.getImageViews()[i], g_Swapchain.getSize(), g_RenderPass);
	}

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

