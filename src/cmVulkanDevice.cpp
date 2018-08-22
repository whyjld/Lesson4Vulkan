/**
	\brief Vulkan instance·â×°Àà
*/
//------------------------------------------------------------------------------
#include "cmVulkanInstance.h"
#include "cmVulkanSurface.h"
#include "cmVulkanDevice.h"
#include <algorithm>
#include <fstream>
//------------------------------------------------------------------------------
#if defined(max)
#undef max
#endif
//------------------------------------------------------------------------------
CM_BEGIN
//------------------------------------------------------------------------------
VulkanDevice::VulkanDevice()
: f_PhysicalDevice(nullptr)
, f_Device(VK_NULL_HANDLE)
, f_Usage(new int(1))
{
	memset(f_Queues, 0, sizeof(f_Queues));
}
//------------------------------------------------------------------------------
VulkanDevice::VulkanDevice(VkDevice device, const std::vector<QueueInfo>& queueInfos, VulkanPhysicalDevice& physicalDevice)
: f_PhysicalDevice(&physicalDevice)
, f_Device(device)
, f_Usage(new int(1))
{
	volkLoadDeviceTable(&f_DeviceFunctions, f_Device);

	memset(f_Queues, 0, sizeof(f_Queues));

	for (auto q : queueInfos)
	{
		VkCommandPoolCreateInfo cmdPoolCreateInfo =
		{
			VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,     // VkStructureType              sType
			nullptr,                                        // const void*                  pNext
			VK_COMMAND_POOL_CREATE_TRANSIENT_BIT | VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT, // VkCommandPoolCreateFlags     flags
			q.Family                                        // uint32_t                     queueFamilyIndex
		};

		VkCommandPool pool = VK_NULL_HANDLE;

		if (f_DeviceFunctions.vkCreateCommandPool(f_Device, &cmdPoolCreateInfo, nullptr, &pool) != VK_SUCCESS)
		{
			THROW("Could not create graphics command pool!");
		}
		for (auto t : q.Types)
		{
			f_Queues[t].Family = q.Family;
			f_Queues[t].Count = q.Priorities.size();
			f_Queues[t].Pool = pool;
		}
	}
}
//------------------------------------------------------------------------------
VulkanDevice::VulkanDevice(VulkanDevice& r)
	: f_PhysicalDevice(r.f_PhysicalDevice), f_DeviceFunctions(), f_Device(r.f_Device), f_Usage(r.f_Usage)
{
	memcpy(f_Queues, r.f_Queues, sizeof(f_Queues));

	++(*f_Usage);
}
//------------------------------------------------------------------------------
VulkanDevice::~VulkanDevice()
{
	if (0 == --(*f_Usage))
	{
		if (VK_NULL_HANDLE != f_Device)
		{
			f_DeviceFunctions.vkDeviceWaitIdle(f_Device);
			if (VK_NULL_HANDLE != f_Queues[qtPresent].Pool && f_Queues[qtPresent].Pool != f_Queues[qtGraphics].Pool)
			{
				f_DeviceFunctions.vkDestroyCommandPool(f_Device, f_Queues[qtPresent].Pool, nullptr);
			}
			f_DeviceFunctions.vkDestroyCommandPool(f_Device, f_Queues[qtGraphics].Pool, nullptr);
			f_DeviceFunctions.vkDestroyDevice(f_Device, nullptr);
		}
		delete f_Usage;
	}
}
//------------------------------------------------------------------------------
VulkanDevice& VulkanDevice::operator=(const VulkanDevice& r)
{
	if (this != &r)
	{
		if (0 == --(*f_Usage))
		{
			if (0 != f_Device)
			{
				f_DeviceFunctions.vkDeviceWaitIdle(f_Device);
				if (VK_NULL_HANDLE != f_Queues[qtPresent].Pool)
				{
					f_DeviceFunctions.vkDestroyCommandPool(f_Device, f_Queues[qtPresent].Pool, nullptr);
				}
				f_DeviceFunctions.vkDestroyCommandPool(f_Device, f_Queues[qtGraphics].Pool, nullptr);
				f_DeviceFunctions.vkDestroyDevice(f_Device, nullptr);
			}
			delete f_Usage;
		}
		memcpy(f_Queues, r.f_Queues, sizeof(f_Queues));
		f_PhysicalDevice = r.f_PhysicalDevice;
		f_DeviceFunctions = r.f_DeviceFunctions;
		f_Device = r.f_Device;
		f_Usage = r.f_Usage;
		++(*f_Usage);
	}
	return *this;
}
//------------------------------------------------------------------------------
VulkanDevice& VulkanDevice::operator=(VulkanDevice&& r)
{
	if (this != &r)
	{
		std::swap(f_PhysicalDevice, r.f_PhysicalDevice);
		std::swap(f_DeviceFunctions, r.f_DeviceFunctions);
		std::swap(f_Device, r.f_Device);
		for (size_t i = 0; i < qtCount; ++i)
		{
			std::swap(f_Queues[i], r.f_Queues[i]);
		}
		std::swap(f_Usage, r.f_Usage);
	}
	return *this;
}
//------------------------------------------------------------------------------
const VulkanPhysicalDevice& VulkanDevice::GetPhysicalDevice() const
{
	return *f_PhysicalDevice;
}
//------------------------------------------------------------------------------
uint32_t VulkanDevice::GetQueueFamily(QueueType type) const
{
	return f_Queues[type].Family;
}
//------------------------------------------------------------------------------
uint32_t VulkanDevice::GetQueueCount(QueueType type) const
{
	return f_Queues[type].Count;
}
//------------------------------------------------------------------------------
VulkanQueue VulkanDevice::GetQueue(QueueType type, uint32_t queue)
{
	if (queue >= f_Queues[type].Count)
	{
		THROW("Out of queue count.");
	}

	VkQueue vq;
	f_DeviceFunctions.vkGetDeviceQueue(f_Device, f_Queues[type].Family, queue, &vq);
	return VulkanQueue(vq, f_Queues[type].Family, this);
}
//------------------------------------------------------------------------------
VulkanCommandBuffer VulkanDevice::AllocCommandBuffer(QueueType type)
{
	return VulkanCommandBuffer(f_Queues[type].Pool, this);
}
//------------------------------------------------------------------------------
VulkanMemory VulkanDevice::AllocMemory(uint32_t type, VkMemoryPropertyFlagBits mp, VkDeviceSize size)
{
	VkPhysicalDeviceMemoryProperties memory_properties = f_PhysicalDevice->GetMemoryProperties();

	for (uint32_t i = 0; i < memory_properties.memoryTypeCount; ++i)
	{
		if ((type & (1 << i)) && (memory_properties.memoryTypes[i].propertyFlags & mp))
		{
			VkMemoryAllocateInfo memory_allocate_info =
			{
				VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,     // VkStructureType                        sType
				nullptr,                                    // const void                            *pNext
				size,                                       // VkDeviceSize                           allocationSize
				i                                           // uint32_t                               memoryTypeIndex
			};

			VkDeviceMemory memory;
			if (f_DeviceFunctions.vkAllocateMemory(f_Device, &memory_allocate_info, nullptr, &memory) == VK_SUCCESS)
			{
				return VulkanMemory(memory, size, this);
			}
		}
	}
	THROW("Could not allocate memory.");
}
//------------------------------------------------------------------------------
VulkanMemory VulkanDevice::AllocAndBindMemory(std::vector<VulkanBuffer*>& buffers, VkMemoryPropertyFlagBits mp)
{
	uint32_t type = ~uint32_t(0);
	VkDeviceSize size = 0;

	std::vector<VkDeviceSize> offsets;
	for (const auto& b : buffers)
	{
		VkMemoryRequirements buffer_memory_requirements;
		f_DeviceFunctions.vkGetBufferMemoryRequirements(f_Device, b->f_Buffer, &buffer_memory_requirements);

		type &= buffer_memory_requirements.memoryTypeBits;
		if (0 == type)
		{
			THROW("Could not find memory type for buffers.")
		}

		VkDeviceSize offset = (size + buffer_memory_requirements.alignment - 1) / buffer_memory_requirements.alignment * buffer_memory_requirements.alignment;
		offsets.push_back(offset);
		size = offset + buffer_memory_requirements.size;
	}

	VulkanMemory ret = AllocMemory(type, mp, size);
	for (size_t i = 0; i < buffers.size(); ++i)
	{
		if (!buffers[i]->innerBind(ret, offsets[i]))
		{
			THROW("Could not bind buffer to memory.");
		}
	}
	return ret;
}
//------------------------------------------------------------------------------
VulkanMemory VulkanDevice::AllocAndBindMemory(std::vector<VulkanImage*>& images, VkMemoryPropertyFlagBits mp)
{
	uint32_t type = ~uint32_t(0);
	VkDeviceSize size = 0;

	std::vector<VkDeviceSize> offsets;
	for (const auto& i : images)
	{
		VkMemoryRequirements image_memory_requirements;
		f_DeviceFunctions.vkGetImageMemoryRequirements(f_Device, i->f_Image, &image_memory_requirements);

		type &= image_memory_requirements.memoryTypeBits;
		if (0 == type)
		{
			THROW("Could not find memory type for buffers.")
		}

		VkDeviceSize offset = (size + image_memory_requirements.alignment - 1) / image_memory_requirements.alignment * image_memory_requirements.alignment;
		offsets.push_back(offset);
		size = offset + image_memory_requirements.size;
	}

	VulkanMemory ret = AllocMemory(type, mp, size);
	for (size_t i = 0; i < images.size(); ++i)
	{
		if (!images[i]->innerBind(ret, offsets[i]))
		{
			THROW("Could not bind buffer to memory.");
		}
	}
	return ret;
}
//------------------------------------------------------------------------------
VulkanFence VulkanDevice::CreateFence()
{
	VkFenceCreateInfo fence_create_info =
	{
		VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,              // VkStructureType                sType
		nullptr,                                          // const void                    *pNext
		VK_FENCE_CREATE_SIGNALED_BIT                      // VkFenceCreateFlags             flags
	};

	VkFence fence;
	if (f_DeviceFunctions.vkCreateFence(f_Device, &fence_create_info, nullptr, &fence) != VK_SUCCESS)
	{
		THROW("Could not create a fence!");
	}
	return VulkanFence(fence, this);
}
//------------------------------------------------------------------------------
VulkanSemaphore VulkanDevice::CreateSemaphore()
{
	VkSemaphoreCreateInfo semaphoreCreateInfo =
	{
		VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,      // VkStructureType          sType
		nullptr,                                      // const void*              pNext
		0                                             // VkSemaphoreCreateFlags   flags
	};

	VkSemaphore semaphore;
	if (f_DeviceFunctions.vkCreateSemaphore(f_Device, &semaphoreCreateInfo, nullptr, &semaphore) != VK_SUCCESS)
	{
		THROW("Could not create semaphores!");
	}

	return VulkanSemaphore(semaphore, this);
}
//------------------------------------------------------------------------------
VulkanSwapchain VulkanDevice::CreateSwapchain(const VulkanSurface& surface, uint32_t width, uint32_t height, uint32_t image, VulkanSwapchain* oldSwapchain)
{
	f_DeviceFunctions.vkDeviceWaitIdle(f_Device);

	auto scs = f_PhysicalDevice->GetSurfaceCapabilities(surface);
	VkImageUsageFlags usage = GetSwapChainUsageFlags(scs);
	if (static_cast<VkImageUsageFlags>(-1) == usage)
	{
		THROW("Could not get swapchain usage flags!");
	}
	VkPresentModeKHR presentMode = GetSwapChainPresentMode(surface);
	if (static_cast<VkPresentModeKHR>(-1) == presentMode)
	{
		THROW("Could not get swapchain present mode!");
	}

	VkSurfaceFormatKHR surfaceFormat = GetSwapChainFormat(surface);

	if (0 == image)
	{
		image = scs.minImageCount + 1;
		if ((scs.maxImageCount > 0) && (image > scs.maxImageCount))
		{
			image = scs.maxImageCount;
		}
	}
	else if (image < scs.minImageCount)
	{
		image = scs.minImageCount;
	}
	
	VkSwapchainKHR iOldSwapchain = (oldSwapchain != nullptr) ? oldSwapchain->f_Swapchain : VK_NULL_HANDLE;
	VkSwapchainCreateInfoKHR swapchainCreateInfo =
	{
		VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,  // VkStructureType                sType
		nullptr,                                      // const void                    *pNext
		0,                                            // VkSwapchainCreateFlagsKHR      flags
		surface.f_Surface,                            // VkSurfaceKHR                   surface
		image,                                        // uint32_t                       minImageCount
		surfaceFormat.format,                         // VkFormat                       imageFormat
		surfaceFormat.colorSpace,                     // VkColorSpaceKHR                imageColorSpace
		GetSwapChainImageSize(scs, width, height),    // VkExtent2D                     imageExtent
		1,                                            // uint32_t                       imageArrayLayers
		usage,                                        // VkImageUsageFlags              imageUsage
		VK_SHARING_MODE_EXCLUSIVE,                    // VkSharingMode                  imageSharingMode
		0,                                            // uint32_t                       queueFamilyIndexCount
		nullptr,                                      // const uint32_t                *pQueueFamilyIndices
		GetSwapChainTransform(scs),                   // VkSurfaceTransformFlagBitsKHR  preTransform
		VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,            // VkCompositeAlphaFlagBitsKHR    compositeAlpha
		presentMode,                                  // VkPresentModeKHR               presentMode
		VK_TRUE,                                      // VkBool32                       clipped
		iOldSwapchain                                 // VkSwapchainKHR                 oldSwapchain
	};

	VkSwapchainKHR swapchain;
	if (f_DeviceFunctions.vkCreateSwapchainKHR(f_Device, &swapchainCreateInfo, nullptr, &swapchain) != VK_SUCCESS)
	{
		THROW("Could not create swapchain!");
	}
	if (iOldSwapchain != VK_NULL_HANDLE)
	{
		oldSwapchain->f_Swapchain = VK_NULL_HANDLE;
		f_DeviceFunctions.vkDestroySwapchainKHR(f_Device, iOldSwapchain, nullptr);
	}
	return VulkanSwapchain(swapchain, swapchainCreateInfo, this);
}
//------------------------------------------------------------------------------
VulkanRenderPass VulkanDevice::CreateRenderPass(VkFormat color, uint32_t count, VkFormat depth)
{
	std::vector<VkAttachmentDescription> ads;
	std::vector<VkAttachmentReference> cars;
	for (uint32_t i = 0; i < count;++i)
	{
		ads.push_back(VkAttachmentDescription(
		{
			0,                                          // VkAttachmentDescriptionFlags   flags
			color,                                      // VkFormat                       format
			VK_SAMPLE_COUNT_1_BIT,                      // VkSampleCountFlagBits          samples
			VK_ATTACHMENT_LOAD_OP_CLEAR,                // VkAttachmentLoadOp             loadOp
			VK_ATTACHMENT_STORE_OP_STORE,               // VkAttachmentStoreOp            storeOp
			VK_ATTACHMENT_LOAD_OP_DONT_CARE,            // VkAttachmentLoadOp             stencilLoadOp
			VK_ATTACHMENT_STORE_OP_DONT_CARE,           // VkAttachmentStoreOp            stencilStoreOp
			VK_IMAGE_LAYOUT_UNDEFINED,                  // VkImageLayout                  initialLayout;
			VK_IMAGE_LAYOUT_PRESENT_SRC_KHR             // VkImageLayout                  finalLayout
		}));

		cars.push_back(VkAttachmentReference(
		{
			i,                                          // uint32_t                       attachment
			VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL    // VkImageLayout                  layout
		}));
	}

	VkAttachmentReference dard = 
	{
		count,                                      // uint32_t                       attachment
		VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL    // VkImageLayout                  layout
	};
	VkAttachmentReference* dar = nullptr;
	if (VK_FORMAT_UNDEFINED != depth)
	{
		ads.push_back(VkAttachmentDescription(
		{
			0,                                          // VkAttachmentDescriptionFlags   flags
			depth,                                      // VkFormat                       format
			VK_SAMPLE_COUNT_1_BIT,                      // VkSampleCountFlagBits          samples
			VK_ATTACHMENT_LOAD_OP_CLEAR,                // VkAttachmentLoadOp             loadOp
			VK_ATTACHMENT_STORE_OP_STORE,               // VkAttachmentStoreOp            storeOp
			VK_ATTACHMENT_LOAD_OP_DONT_CARE,            // VkAttachmentLoadOp             stencilLoadOp
			VK_ATTACHMENT_STORE_OP_DONT_CARE,           // VkAttachmentStoreOp            stencilStoreOp
			VK_IMAGE_LAYOUT_UNDEFINED,                  // VkImageLayout                  initialLayout;
			VK_IMAGE_LAYOUT_PRESENT_SRC_KHR             // VkImageLayout                  finalLayout
		}));

		dar = &dard;
	}

	VkSubpassDescription spd[] =
	{
		{
			0,                                          // VkSubpassDescriptionFlags      flags
			VK_PIPELINE_BIND_POINT_GRAPHICS,            // VkPipelineBindPoint            pipelineBindPoint
			0,                                          // uint32_t                       inputAttachmentCount
			nullptr,                                    // const VkAttachmentReference   *pInputAttachments
			cars.size(),                                 // uint32_t                       colorAttachmentCount
			&cars[0],                                    // const VkAttachmentReference   *pColorAttachments
			nullptr,                                    // const VkAttachmentReference   *pResolveAttachments
			dar,                                        // const VkAttachmentReference   *pDepthStencilAttachment
			0,                                          // uint32_t                       preserveAttachmentCount
			nullptr                                     // const uint32_t*                pPreserveAttachments
		}
	};

	VkRenderPassCreateInfo render_pass_create_info = {
		VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,    // VkStructureType                sType
		nullptr,                                      // const void                    *pNext
		0,                                            // VkRenderPassCreateFlags        flags
		ads.size(),                                   // uint32_t                       attachmentCount
		&ads[0],                                      // const VkAttachmentDescription *pAttachments
		1,                                            // uint32_t                       subpassCount
		spd,                                          // const VkSubpassDescription    *pSubpasses
		0,                                            // uint32_t                       dependencyCount
		nullptr                                       // const VkSubpassDependency     *pDependencies
	};

	VkRenderPass renderPass;
	if (f_DeviceFunctions.vkCreateRenderPass(f_Device, &render_pass_create_info, nullptr, &renderPass) != VK_SUCCESS)
	{
		THROW("Could not create render pass!")
	}

	return VulkanRenderPass(renderPass, this);
}
//------------------------------------------------------------------------------
VulkanFramebuffer VulkanDevice::CreateFramebuffer(VkImage image, const VkExtent2D& size, const VulkanRenderPass& renderpass)
{
	VkFramebufferCreateInfo framebuffer_create_info =
	{
		VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,  // VkStructureType                sType
		nullptr,                                    // const void                    *pNext
		0,                                          // VkFramebufferCreateFlags       flags
		renderpass.f_RenderPass,                    // VkRenderPass                   renderPass
		1,                                          // uint32_t                       attachmentCount
		&image,                                     // const VkImageView             *pAttachments
		size.width,                                 // uint32_t                       width
		size.height,                                // uint32_t                       height
		1                                           // uint32_t                       layers
	};

	VkFramebuffer fb;
	if (f_DeviceFunctions.vkCreateFramebuffer(f_Device, &framebuffer_create_info, nullptr, &fb) != VK_SUCCESS)
	{
		THROW("Could not create a framebuffer!");
	}
	return VulkanFramebuffer(fb, this);
}
//------------------------------------------------------------------------------
VulkanShader VulkanDevice::CreateShader(const char* binary, size_t length)
{
	VkShaderModuleCreateInfo shader_module_create_info =
	{
		VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,  // VkStructureType                sType
		nullptr,                                      // const void                    *pNext
		0,                                            // VkShaderModuleCreateFlags      flags
		length,                                       // size_t                         codeSize
		reinterpret_cast<const uint32_t*>(binary)     // const uint32_t                *pCode
	};

	VkShaderModule sm;
	if (f_DeviceFunctions.vkCreateShaderModule(f_Device, &shader_module_create_info, nullptr, &sm) != VK_SUCCESS)
	{
		THROW("Could not create shader module from a file!");
	}

	return VulkanShader(sm, this);
}
//------------------------------------------------------------------------------
VulkanShader VulkanDevice::CreateShader(const std::string& filename)
{
	std::ifstream inf(filename, std::ios::binary | std::ios::in);
	if (!inf)
	{
		THROW("Could not read shader from file.");
	}
	inf.seekg(0, std::ios::end);
	std::vector<char> buf(static_cast<size_t>(inf.tellg()));
	inf.seekg(0, std::ios::beg);

	inf.read(&buf[0], buf.size());

	return CreateShader(&buf[0], buf.size());
}
//------------------------------------------------------------------------------
VulkanBuffer VulkanDevice::CreateBuffer(VkBufferUsageFlags usage, VkSharingMode sharingMode, VkDeviceSize size)
{
	VkBufferCreateInfo buffer_create_info =
	{
		VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,             // VkStructureType        sType
		nullptr,                                          // const void            *pNext
		0,                                                // VkBufferCreateFlags    flags
		size,                                             // VkDeviceSize           size
		usage,                                            // VkBufferUsageFlags     usage
		sharingMode,                                      // VkSharingMode          sharingMode
		0,                                                // uint32_t               queueFamilyIndexCount
		nullptr                                           // const uint32_t        *pQueueFamilyIndices
	};

	VkBuffer buffer;
	if (f_DeviceFunctions.vkCreateBuffer(f_Device, &buffer_create_info, nullptr, &buffer) != VK_SUCCESS)
	{
		THROW("Could not create a vertex buffer!");
	}
	return VulkanBuffer(buffer, this);
}
//------------------------------------------------------------------------------
VulkanImage VulkanDevice::CreateImage(VkImageType type, VkFormat format, VkExtent3D size, uint32_t mips, uint32_t arrays, VkSampleCountFlagBits samples, VkImageTiling tiling, VkImageUsageFlags usage)
{
	VkImageCreateInfo image_create_info =
	{
		VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,  // VkStructureType        sType;
		nullptr,                              // const void            *pNext
		0,                                    // VkImageCreateFlags     flags
		type,                                 // VkImageType            imageType
		format,                               // VkFormat               format
		size,                                 // VkExtent3D             extent
		mips,                                 // uint32_t               mipLevels
		arrays,                               // uint32_t               arrayLayers
		samples,                              // VkSampleCountFlagBits  samples
		tiling,                               // VkImageTiling          tiling
		usage,                                // VkImageUsageFlags      usage
		VK_SHARING_MODE_EXCLUSIVE,            // VkSharingMode          sharingMode
		0,                                    // uint32_t               queueFamilyIndexCount
		nullptr,                              // const uint32_t        *pQueueFamilyIndices
		VK_IMAGE_LAYOUT_UNDEFINED             // VkImageLayout          initialLayout
	};

	VkImage image;
	if (f_DeviceFunctions.vkCreateImage(f_Device, &image_create_info, nullptr, &image) != VK_SUCCESS)
	{
		THROW("Could not create a image!")
	}
	return VulkanImage(image, image_create_info, this);
}
//------------------------------------------------------------------------------
VulkanSampler VulkanDevice::CreateSampler(VkFilter filter, VkSamplerMipmapMode mipmapMode, VkSamplerAddressMode addressMode, float maxAnisotropy)
{
	VkSamplerCreateInfo sampler_create_info =
	{
		VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,                // VkStructureType            sType
		nullptr,                                              // const void*                pNext
		0,                                                    // VkSamplerCreateFlags       flags
		filter,                                               // VkFilter                   magFilter
		filter,                                               // VkFilter                   minFilter
		mipmapMode,                                           // VkSamplerMipmapMode        mipmapMode
		addressMode,                                          // VkSamplerAddressMode       addressModeU
		addressMode,                                          // VkSamplerAddressMode       addressModeV
		addressMode,                                          // VkSamplerAddressMode       addressModeW
		0.0f,                                                 // float                      mipLodBias
		maxAnisotropy > 1.0f,                                 // VkBool32                   anisotropyEnable
		std::max(1.0f, maxAnisotropy),                        // float                      maxAnisotropy
		VK_FALSE,                                             // VkBool32                   compareEnable
		VK_COMPARE_OP_ALWAYS,                                 // VkCompareOp                compareOp
		0.0f,                                                 // float                      minLod
		0.0f,                                                 // float                      maxLod
		VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK,              // VkBorderColor              borderColor
		VK_FALSE                                              // VkBool32                   unnormalizedCoordinates
	};

	VkSampler sampler;
	if (f_DeviceFunctions.vkCreateSampler(f_Device, &sampler_create_info, nullptr, &sampler) != VK_SUCCESS)
	{
		THROW("Could not create sampler.");
	}
	return VulkanSampler(sampler, this);
}
//------------------------------------------------------------------------------
VulkanPipelineLayout VulkanDevice::CreatePipelineLayout(const std::vector<VulkanDescriptorSetLayout>& descriptorSetLayouts)
{
	std::vector<VkDescriptorSetLayout> dsls;
	for (const auto& l : descriptorSetLayouts)
	{
		dsls.push_back(l.f_Layout);
	}
	const VkDescriptorSetLayout* setLayouts = dsls.empty() ? nullptr : &dsls[0];
	VkPipelineLayoutCreateInfo layout_create_info =
	{
		VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,  // VkStructureType                sType
		nullptr,                                        // const void                    *pNext
		0,                                              // VkPipelineLayoutCreateFlags    flags
		dsls.size(),                                    // uint32_t                       setLayoutCount
		setLayouts,                                     // const VkDescriptorSetLayout   *pSetLayouts
		0,                                              // uint32_t                       pushConstantRangeCount
		nullptr                                         // const VkPushConstantRange     *pPushConstantRanges
	};

	VkPipelineLayout layout;
	if (f_DeviceFunctions.vkCreatePipelineLayout(f_Device, &layout_create_info, nullptr, &layout) != VK_SUCCESS)
	{
		THROW("Could not create pipeline layout!");
	}

	return VulkanPipelineLayout(layout, this);
}
//------------------------------------------------------------------------------
VulkanDescriptorSetLayout VulkanDevice::CreateDescriptorSetLayout(std::vector<VkDescriptorSetLayoutBinding> bindings, uint32_t maxSets)
{
	VkDescriptorSetLayoutCreateInfo descriptor_set_layout_create_info =
	{
		VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,  // VkStructureType                      sType
		nullptr,                                              // const void                          *pNext
		0,                                                    // VkDescriptorSetLayoutCreateFlags     flags
		bindings.size(),                                      // uint32_t                             bindingCount
		&bindings[0],                                         // const VkDescriptorSetLayoutBinding  *pBindings
	};

	VkDescriptorSetLayout dsl;
	if (f_DeviceFunctions.vkCreateDescriptorSetLayout(f_Device, &descriptor_set_layout_create_info, nullptr, &dsl) != VK_SUCCESS)
	{
		THROW("Could not create descriptor set layout.");
	}

	std::vector<VkDescriptorPoolSize> pool_sizes;
	for (const auto& binding : bindings)
	{
		pool_sizes.push_back(VkDescriptorPoolSize(
		{
			binding.descriptorType,
			binding.descriptorCount,
		}));
	}

	VkDescriptorPoolCreateInfo descriptor_pool_create_info = 
	{
		VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,  // VkStructureType                sType
		nullptr,                                        // const void                    *pNext
		0,                                              // VkDescriptorPoolCreateFlags    flags
		maxSets,                                        // uint32_t                       maxSets
		pool_sizes.size(),                              // uint32_t                       poolSizeCount
		&pool_sizes[0],                                 // const VkDescriptorPoolSize    *pPoolSizes
	};

	VkDescriptorPool pool;
	if (vkCreateDescriptorPool(f_Device, &descriptor_pool_create_info, nullptr, &pool) != VK_SUCCESS)
	{
		vkDestroyDescriptorSetLayout(f_Device, dsl, nullptr);
		THROW("Could not create descriptor pool!");
	}

	return VulkanDescriptorSetLayout(dsl, pool, this);
}
//------------------------------------------------------------------------------
VulkanGraphicsPipeline VulkanDevice::CreateGraphicsPipeline(const VulkanRenderPass& renderpass,
	const VulkanPipelineLayout& layout,
	const std::vector<ShaderStageInfo>& stages, 
	VkPrimitiveTopology pt, 
	const VkRect2D& viewport,
	const std::vector<VkVertexInputBindingDescription>& vertexBindings,
	const std::vector<VkVertexInputAttributeDescription>& vertexAttributes)
{
	if (stages.size() < 2)
	{
		THROW("Have no enough shader for pipeline.");
	}

	std::vector<VkPipelineShaderStageCreateInfo> shader_stage_create_infos;
	for (auto& s : stages)
	{
		shader_stage_create_infos.push_back(VkPipelineShaderStageCreateInfo(
		{
			VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,        // VkStructureType                                sType
			nullptr,                                                    // const void                                    *pNext
			0,                                                          // VkPipelineShaderStageCreateFlags               flags
			s.Stage,                                                    // VkShaderStageFlagBits                          stage
			s.Shader->f_Shader,                                         // VkShaderModule                                 module
			s.Entry,                                                    // const char                                    *pName
			nullptr                                                     // const VkSpecializationInfo                    *pSpecializationInfo
		}));
	}

	VkPipelineVertexInputStateCreateInfo vertex_input_state_create_info =
	{
		VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,    // VkStructureType                                sType
		nullptr,                                                      // const void                                    *pNext
		0,                                                            // VkPipelineVertexInputStateCreateFlags          flags;
		static_cast<uint32_t>(vertexBindings.size()),                 // uint32_t                                       vertexBindingDescriptionCount
		vertexBindings.size() > 0 ? &vertexBindings[0] : nullptr,     // const VkVertexInputBindingDescription         *pVertexBindingDescriptions
		static_cast<uint32_t>(vertexAttributes.size()),               // uint32_t                                       vertexAttributeDescriptionCount
		vertexAttributes.size() > 0 ? &vertexAttributes[0] : nullptr, // const VkVertexInputAttributeDescription       *pVertexAttributeDescriptions
	};

	VkPipelineInputAssemblyStateCreateInfo input_assembly_state_create_info =
	{
		VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,  // VkStructureType                                sType
		nullptr,                                                      // const void                                    *pNext
		0,                                                            // VkPipelineInputAssemblyStateCreateFlags        flags
		pt,                                                           // VkPrimitiveTopology                            topology
		VK_FALSE                                                      // VkBool32                                       primitiveRestartEnable
	};

	VkViewport viewports[] =
	{
		{
			static_cast<float>(viewport.offset.x),                        // float                                          x
			static_cast<float>(viewport.offset.x),                        // float                                          y
			static_cast<float>(viewport.extent.width),                    // float                                          width
			static_cast<float>(viewport.extent.height),                   // float                                          height
			0.0f,                                                         // float                                          minDepth
			1.0f                                                          // float                                          maxDepth
		}
	};

	VkRect2D scissors[] =
	{
		viewport,
	};

	VkPipelineViewportStateCreateInfo viewport_state_create_info =
	{
		VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,        // VkStructureType                                sType
		nullptr,                                                      // const void                                    *pNext
		0,                                                            // VkPipelineViewportStateCreateFlags             flags
		1,                                                            // uint32_t                                       viewportCount
		viewports,                                                    // const VkViewport                              *pViewports
		1,                                                            // uint32_t                                       scissorCount
		scissors,                                                     // const VkRect2D                                *pScissors
	};

	VkPipelineRasterizationStateCreateInfo rasterization_state_create_info = {
		VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,   // VkStructureType                                sType
		nullptr,                                                      // const void                                    *pNext
		0,                                                            // VkPipelineRasterizationStateCreateFlags        flags
		VK_FALSE,                                                     // VkBool32                                       depthClampEnable
		VK_FALSE,                                                     // VkBool32                                       rasterizerDiscardEnable
		VK_POLYGON_MODE_FILL,                                         // VkPolygonMode                                  polygonMode
		VK_CULL_MODE_BACK_BIT,                                        // VkCullModeFlags                                cullMode
		VK_FRONT_FACE_COUNTER_CLOCKWISE,                              // VkFrontFace                                    frontFace
		VK_FALSE,                                                     // VkBool32                                       depthBiasEnable
		0.0f,                                                         // float                                          depthBiasConstantFactor
		0.0f,                                                         // float                                          depthBiasClamp
		0.0f,                                                         // float                                          depthBiasSlopeFactor
		1.0f                                                          // float                                          lineWidth
	};

	VkPipelineMultisampleStateCreateInfo multisample_state_create_info = {
		VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,     // VkStructureType                                sType
		nullptr,                                                      // const void                                    *pNext
		0,                                                            // VkPipelineMultisampleStateCreateFlags          flags
		VK_SAMPLE_COUNT_1_BIT,                                        // VkSampleCountFlagBits                          rasterizationSamples
		VK_FALSE,                                                     // VkBool32                                       sampleShadingEnable
		1.0f,                                                         // float                                          minSampleShading
		nullptr,                                                      // const VkSampleMask                            *pSampleMask
		VK_FALSE,                                                     // VkBool32                                       alphaToCoverageEnable
		VK_FALSE                                                      // VkBool32                                       alphaToOneEnable
	};

	VkPipelineColorBlendAttachmentState color_blend_attachment_state = {
		VK_FALSE,                                                     // VkBool32                                       blendEnable
		VK_BLEND_FACTOR_ONE,                                          // VkBlendFactor                                  srcColorBlendFactor
		VK_BLEND_FACTOR_ZERO,                                         // VkBlendFactor                                  dstColorBlendFactor
		VK_BLEND_OP_ADD,                                              // VkBlendOp                                      colorBlendOp
		VK_BLEND_FACTOR_ONE,                                          // VkBlendFactor                                  srcAlphaBlendFactor
		VK_BLEND_FACTOR_ZERO,                                         // VkBlendFactor                                  dstAlphaBlendFactor
		VK_BLEND_OP_ADD,                                              // VkBlendOp                                      alphaBlendOp
		VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |         // VkColorComponentFlags                          colorWriteMask
		VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT
	};

	VkPipelineColorBlendStateCreateInfo color_blend_state_create_info = {
		VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,     // VkStructureType                                sType
		nullptr,                                                      // const void                                    *pNext
		0,                                                            // VkPipelineColorBlendStateCreateFlags           flags
		VK_FALSE,                                                     // VkBool32                                       logicOpEnable
		VK_LOGIC_OP_COPY,                                             // VkLogicOp                                      logicOp
		1,                                                            // uint32_t                                       attachmentCount
		&color_blend_attachment_state,                                // const VkPipelineColorBlendAttachmentState     *pAttachments
		{ 0.0f, 0.0f, 0.0f, 0.0f }                                    // float                                          blendConstants[4]
	};

	VkGraphicsPipelineCreateInfo pipeline_create_info =
	{
		VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,              // VkStructureType                                sType
		nullptr,                                                      // const void                                    *pNext
		0,                                                            // VkPipelineCreateFlags                          flags
		static_cast<uint32_t>(shader_stage_create_infos.size()),      // uint32_t                                       stageCount
		&shader_stage_create_infos[0],                                // const VkPipelineShaderStageCreateInfo         *pStages
		&vertex_input_state_create_info,                              // const VkPipelineVertexInputStateCreateInfo    *pVertexInputState;
		&input_assembly_state_create_info,                            // const VkPipelineInputAssemblyStateCreateInfo  *pInputAssemblyState
		nullptr,                                                      // const VkPipelineTessellationStateCreateInfo   *pTessellationState
		&viewport_state_create_info,                                  // const VkPipelineViewportStateCreateInfo       *pViewportState
		&rasterization_state_create_info,                             // const VkPipelineRasterizationStateCreateInfo  *pRasterizationState
		&multisample_state_create_info,                               // const VkPipelineMultisampleStateCreateInfo    *pMultisampleState
		nullptr,                                                      // const VkPipelineDepthStencilStateCreateInfo   *pDepthStencilState
		&color_blend_state_create_info,                               // const VkPipelineColorBlendStateCreateInfo     *pColorBlendState
		nullptr,                                                      // const VkPipelineDynamicStateCreateInfo        *pDynamicState
		layout.f_PipelineLayout,                                      // VkPipelineLayout                               layout
		renderpass.f_RenderPass,                                      // VkRenderPass                                   renderPass
		0,                                                            // uint32_t                                       subpass
		VK_NULL_HANDLE,                                               // VkPipeline                                     basePipelineHandle
		-1                                                            // int32_t                                        basePipelineIndex
	};

	VkPipeline pipeline;
	if (f_DeviceFunctions.vkCreateGraphicsPipelines(f_Device, VK_NULL_HANDLE, 1, &pipeline_create_info, nullptr, &pipeline) != VK_SUCCESS)
	{
		THROW("Could not create graphics pipeline!");
	}
	return VulkanGraphicsPipeline(pipeline, this);
}
//------------------------------------------------------------------------------
VkResult VulkanDevice::WaitForFences(const std::vector<VulkanFence>& fences, bool waitAll, uint64_t timeout)
{
	std::vector<VkFence> vkfences;
	for (auto& v : fences)
	{
		vkfences.push_back(v.f_Fence);
	}
	return f_DeviceFunctions.vkWaitForFences(f_Device, vkfences.size(), &vkfences[0], waitAll, timeout);
}
//------------------------------------------------------------------------------
VkResult VulkanDevice::ResetFences(const std::vector<VulkanFence>& fences)
{
	std::vector<VkFence> vkfences;
	for (auto& v : fences)
	{
		vkfences.push_back(v.f_Fence);
	}
	return f_DeviceFunctions.vkResetFences(f_Device, vkfences.size(), &vkfences[0]);
}
//------------------------------------------------------------------------------
VkResult VulkanDevice::WaitIdle() const
{
	return f_DeviceFunctions.vkDeviceWaitIdle(f_Device);
}
//------------------------------------------------------------------------------
VkPresentModeKHR VulkanDevice::GetSwapChainPresentMode(const CM::VulkanSurface& surface)
{
	auto pms = f_PhysicalDevice->GetSurfacePresentModes(surface);

	VkPresentModeKHR ret = static_cast<VkPresentModeKHR>(-1);
	for (auto pm : pms)
	{
		if (VK_PRESENT_MODE_MAILBOX_KHR == pm)
		{
			return VK_PRESENT_MODE_MAILBOX_KHR;
		}
		if (VK_PRESENT_MODE_FIFO_KHR == pm)
		{
			ret = VK_PRESENT_MODE_FIFO_KHR;
		}
	}
	return ret;
}
//------------------------------------------------------------------------------
VkSurfaceFormatKHR VulkanDevice::GetSwapChainFormat(const CM::VulkanSurface& surface)
{
	auto sfs = f_PhysicalDevice->GetSurfaceFormats(surface);

	if ((sfs.size() == 1) && (VK_FORMAT_UNDEFINED == sfs[0].format))
	{
		return{ VK_FORMAT_R8G8B8A8_UNORM, VK_COLORSPACE_SRGB_NONLINEAR_KHR };
	}

	for (VkSurfaceFormatKHR &sf : sfs)
	{
		if (VK_FORMAT_R8G8B8A8_UNORM == sf.format)
		{
			return sf;
		}
	}

	return sfs[0];
}
//------------------------------------------------------------------------------
VkExtent2D VulkanDevice::GetSwapChainImageSize(const VkSurfaceCapabilitiesKHR& scs, uint32_t width, uint32_t height)
{
	if (scs.currentExtent.width == -1)
	{
		VkExtent2D imageSize =
		{
			width, height
		};
		if (imageSize.width < scs.minImageExtent.width)
		{
			imageSize.width = scs.minImageExtent.width;
		}
		if (imageSize.height < scs.minImageExtent.height)
		{
			imageSize.height = scs.minImageExtent.height;
		}
		if (imageSize.width > scs.maxImageExtent.width)
		{
			imageSize.width = scs.maxImageExtent.width;
		}
		if (imageSize.height > scs.maxImageExtent.height)
		{
			imageSize.height = scs.maxImageExtent.height;
		}
		return imageSize;
	}
	return scs.currentExtent;
}
//------------------------------------------------------------------------------
VkImageUsageFlags VulkanDevice::GetSwapChainUsageFlags(const VkSurfaceCapabilitiesKHR& scs)
{
	if (scs.supportedUsageFlags & VK_IMAGE_USAGE_TRANSFER_DST_BIT)
	{
		return VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
	}
	return static_cast<VkImageUsageFlags>(-1);
}
//------------------------------------------------------------------------------
VkSurfaceTransformFlagBitsKHR VulkanDevice::GetSwapChainTransform(const VkSurfaceCapabilitiesKHR& scs)
{
	if (0 != (scs.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR))
	{
		return VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
	}
	else
	{
		return scs.currentTransform;
	}
}
//------------------------------------------------------------------------------
CM_END
