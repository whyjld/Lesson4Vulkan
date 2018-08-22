// lesson7.cpp : 缓冲区间复制数据
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

CM::VulkanShader g_VertexShader;
CM::VulkanShader g_FragmentShader;

std::vector<CM::ShaderStageInfo> g_Shaders;

CM::VulkanMemory g_StagingMemory;
CM::VulkanBuffer g_StagingBuffer;

CM::VulkanMemory g_BufferMemory;
CM::VulkanBuffer g_VertexBuffer;

std::vector<CM::BufferInfo> g_Buffers;

struct RenderResource
{
	CM::VulkanFramebuffer Framebuffer;
	CM::VulkanCommandBuffer CommandBuffer;
	CM::VulkanSemaphore ImageAvailable;
	CM::VulkanSemaphore FinishedRendering;
	CM::VulkanFence Fence;
};

CM::VulkanSurface g_Surface;
CM::VulkanSwapchain g_Swapchain;
CM::VulkanRenderPass g_RenderPass;
CM::VulkanGraphicsPipeline g_Pipeline;
std::vector<RenderResource> g_RenderResources;

const char* title = "lesson7";

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

	g_VertexShader = g_Device.CreateShader("Data/shader.vert.spv");
	g_FragmentShader = g_Device.CreateShader("Data/shader.frag.spv");

	g_Shaders.push_back(CM::ShaderStageInfo({ &g_VertexShader, VK_SHADER_STAGE_VERTEX_BIT, "main" }));
	g_Shaders.push_back(CM::ShaderStageInfo({ &g_FragmentShader, VK_SHADER_STAGE_FRAGMENT_BIT, "main" }));

	for (size_t i = 0; i < 2; ++i)
	{
		g_RenderResources.push_back(
		{
			CM::VulkanFramebuffer(),
			g_Device.AllocCommandBuffer(CM::qtPresent),
			g_Device.CreateSemaphore(),
			g_Device.CreateSemaphore(),
			g_Device.CreateFence(),
		});
	}
}

void CreateStagingBuffer()
{
	VkDeviceSize size = 8 * 1024 * 1024;
	g_StagingBuffer = g_Device.CreateBuffer(VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_SHARING_MODE_EXCLUSIVE, size);
	std::vector<CM::VulkanBuffer*> buffers =
	{
		&g_StagingBuffer,
	};
	g_StagingMemory = g_Device.AllocAndBindMemory(buffers, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
}

void CreateVertexBuffer()
{
	float vertexData[] =
	{
		-0.7f, -0.7f, 0.0f, 1.0f,
		1.0f, 0.0f, 0.0f, 0.0f,
		-0.7f, 0.7f, 0.0f, 1.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.7f, -0.7f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.7f, 0.7f, 0.0f, 1.0f,
		0.3f, 0.3f, 0.3f, 0.0f,
	};

	g_VertexBuffer = g_Device.CreateBuffer(VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VK_SHARING_MODE_EXCLUSIVE, sizeof(vertexData));
	std::vector<CM::VulkanBuffer*> buffers =
	{
		&g_VertexBuffer,
	};
	g_BufferMemory = g_Device.AllocAndBindMemory(buffers, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

	void* stagingStart = g_StagingMemory.Map(g_StagingBuffer.getOffset(), g_StagingBuffer.getSize());
	memcpy(stagingStart, vertexData, sizeof(vertexData));
	g_StagingMemory.Flush(g_StagingBuffer.getOffset(), sizeof(vertexData));
	g_StagingMemory.Unmap();

	auto& cb = g_RenderResources[0].CommandBuffer;
	cb.BeginCommandBuffer(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

	std::vector<VkBufferCopy> buffer_copy_infos =
	{
		{
			0,                   // VkDeviceSize                           srcOffset
			0,                   // VkDeviceSize                           dstOffset
			sizeof(vertexData),  // VkDeviceSize                           size
		}
	};
	cb.CopyBuffer(g_StagingBuffer, g_VertexBuffer, buffer_copy_infos);

	VkBufferMemoryBarrier buffer_memory_barrier = g_VertexBuffer.CreateBufferMemoryBarrier(VK_ACCESS_MEMORY_WRITE_BIT, VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT);
	cb.PipelineBarrier(VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_VERTEX_INPUT_BIT, 0, 0, nullptr, 1, &buffer_memory_barrier, 0, nullptr);
	cb.EndCommandBuffer();

	g_GraphicsQueue.Submit(nullptr, 0, cb);
	g_Device.WaitIdle();

	g_Buffers.push_back(CM::BufferInfo({ g_VertexBuffer, 0 }));
}

bool RecordCommandBuffers(RenderResource& rr, uint32_t image)
{
	std::vector<CM::VulkanFence> fences =
	{
		rr.Fence,
	};
	if (VK_SUCCESS != g_Device.WaitForFences(fences, VK_FALSE, 1000000000))
	{
		return false;
	}
	g_Device.ResetFences(fences);

	rr.CommandBuffer.BeginCommandBuffer(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

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
		{ 1.0f, 0.8f, 0.4f, 0.0f },                     // VkClearColorValue              color
	};

	auto images = g_Swapchain.getImages();

	uint32_t graphicsQueueFamily = g_GraphicsQueue.getFamily();
	uint32_t presentQueueFamily = g_PresentQueue.getFamily();

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
			images[image],                              // VkImage                                image
			image_subresource_range                     // VkImageSubresourceRange                subresourceRange
		};
		rr.CommandBuffer.PipelineBarrier(VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0, nullptr, 1, &barrier_from_present_to_clear);
	}

	rr.CommandBuffer.BeginRenderPass(g_RenderPass, rr.Framebuffer, VkRect2D({ { 0, 0 }, g_Swapchain.getSize() }), clear_value);
	rr.CommandBuffer.BindPipeline(g_Pipeline);
	rr.CommandBuffer.BindVertexBuffers(0, g_Buffers);
	rr.CommandBuffer.Draw(4, 1, 0, 0);

	rr.CommandBuffer.EndRenderPass();

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
			images[image],                                  // VkImage                                image
			image_subresource_range                     // VkImageSubresourceRange                subresourceRange
		};
		rr.CommandBuffer.PipelineBarrier(VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, 0, 0, nullptr, 0, nullptr, 1, &barrier_from_clear_to_present);
	}
	rr.CommandBuffer.EndCommandBuffer();

	return true;
}

void Redraw()
{
	static size_t s_ResourceIndex = 0;
	RenderResource& rr = g_RenderResources[(++s_ResourceIndex) % g_RenderResources.size()];

	auto image = g_Swapchain.AcquireNextImage(rr.ImageAvailable);

	rr.Framebuffer = g_Device.CreateFramebuffer(g_Swapchain.getImageViews()[image], g_Swapchain.getSize(), g_RenderPass);

	RecordCommandBuffers(rr, image);

	g_GraphicsQueue.Submit(&rr.ImageAvailable, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, rr.CommandBuffer, &rr.FinishedRendering, &rr.Fence);
	g_PresentQueue.Present(&rr.FinishedRendering, &g_Swapchain, image);
}

void OnRefresh(CM::GLFWwindow* window)
{
	try
	{
		Redraw();
	}
	catch (...)
	{
	}
}

void OnResize(CM::GLFWwindow* window, int width, int height)
{
	std::vector<VkVertexInputBindingDescription> vertexBindingDescriptions =
	{
		{
			0,                                                          // uint32_t                                       binding
			sizeof(float) * 8,                                          // uint32_t                                       stride
			VK_VERTEX_INPUT_RATE_VERTEX                                 // VkVertexInputRate                              inputRate
		}
	};

	std::vector<VkVertexInputAttributeDescription> vertexAttributeDescriptions =
	{
		{
			0,                                                          // uint32_t                                       location
			vertexBindingDescriptions[0].binding,                       // uint32_t                                       binding
			VK_FORMAT_R32G32B32A32_SFLOAT,                              // VkFormat                                       format
			sizeof(float) * 0,                                          // uint32_t                                       offset
		},
		{
			1,                                                          // uint32_t                                       location
			vertexBindingDescriptions[0].binding,                       // uint32_t                                       binding
			VK_FORMAT_R32G32B32A32_SFLOAT,                              // VkFormat                                       format
			sizeof(float) * 4,                                          // uint32_t                                       offset
		}
	};

	g_Swapchain = g_Device.CreateSwapchain(g_Surface, width, height, 2, &g_Swapchain);
	g_RenderPass = g_Device.CreateRenderPass(g_Swapchain.getFormat());
	g_Pipeline = g_Device.CreateGraphicsPipeline(g_RenderPass, g_PipelineLayout, g_Shaders, VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP, VkRect2D({ { 0, 0 }, g_Swapchain.getSize() }), vertexBindingDescriptions, vertexAttributeDescriptions);
}

int _tmain(int argc, _TCHAR* argv[])
{
	if (!InitializeInstance())
	{
		return 1;
	}

	CreateDevice();
	CreateRendererSource();
	CreateStagingBuffer();
	CreateVertexBuffer();

	g_MainWindow.OnRefresh = OnRefresh;
	g_MainWindow.OnResize = OnResize;
	g_MainWindow.Show();

	for (; !g_MainWindow.ShouldClose();)
	{
		Redraw();
		g_GLFW.WaitEvents();
	}

	return 0;
}

