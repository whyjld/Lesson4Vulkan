/**
	\brief Vulkan逻辑设备封装类
*/
//------------------------------------------------------------------------------
#ifndef _CM_VULKAN_DEVICE_h_
#define _CM_VULKAN_DEVICE_h_ "cmVulkanDevice.h"
//------------------------------------------------------------------------------
#include "cmPublic.h"
#include "cmGLFWVulkan.h"
#include "cmVulkanFence.h"
#include "cmVulkanQueue.h"
#include "cmVulkanImage.h"
#include "cmVulkanBuffer.h"
#include "cmVulkanMemory.h"
#include "cmVulkanShader.h"
#include "cmVulkanSampler.h"
#include "cmVulkanSemaphore.h"
#include "cmVulkanSwapchain.h"
#include "cmVulkanRenderPass.h"
#include "cmVulkanFrameBuffer.h"
#include "cmVulkanCommandBuffer.h"
#include "cmVulkanPipelineLayout.h"
#include "cmVulkanGraphicsPipeline.h"
#include "cmVulkanDescriptorSetLayout.h"
#include <vector>
//------------------------------------------------------------------------------
#if defined(_WIN32) && defined(CreateSemaphore)
#undef CreateSemaphore
#endif//_WIN32 CreateSemaphore
//------------------------------------------------------------------------------
CM_BEGIN
//------------------------------------------------------------------------------
class VulkanSurface;
//------------------------------------------------------------------------------
struct ShaderStageInfo
{
	VulkanShader* Shader;
	VkShaderStageFlagBits Stage;
	const char* Entry;
};
//------------------------------------------------------------------------------
class VulkanDevice
{
public:
	friend class VulkanPhysicalDevice;
	friend class VulkanFence;
	friend class VulkanQueue;
	friend class VulkanImage;
	friend class VulkanImageView;
	friend class VulkanSampler;
	friend class VulkanBuffer;
	friend class VulkanMemory;
	friend class VulkanShader;
	friend class VulkanSemaphore;
	friend class VulkanSwapchain;
	friend class VulkanRenderPass;
	friend class VulkanFramebuffer;
	friend class VulkanDescriptorSet;
	friend class VulkanCommandBuffer;
	friend class VulkanPipelineLayout;
	friend class VulkanGraphicsPipeline;
	friend class VulkanDescriptorSetLayout;

	VulkanDevice();
	VulkanDevice(VulkanDevice& r);
	~VulkanDevice();

	VulkanDevice& operator=(const VulkanDevice& r);
	VulkanDevice& operator=(VulkanDevice&& r);

	const VulkanPhysicalDevice& GetPhysicalDevice() const;

	uint32_t GetQueueFamily(QueueType type) const;
	uint32_t GetQueueCount(QueueType type) const;
	VulkanQueue GetQueue(QueueType type, uint32_t queue);
	VulkanCommandBuffer AllocCommandBuffer(QueueType type);
	VulkanMemory AllocMemory(uint32_t type, VkMemoryPropertyFlagBits mp, VkDeviceSize size);
	VulkanMemory AllocAndBindMemory(std::vector<VulkanBuffer*>& buffers, VkMemoryPropertyFlagBits mp);
	VulkanMemory AllocAndBindMemory(std::vector<VulkanImage*>& images, VkMemoryPropertyFlagBits mp);

	VulkanFence CreateFence();
	VulkanSemaphore CreateSemaphore();
	VulkanSwapchain CreateSwapchain(const VulkanSurface& surface, uint32_t width, uint32_t height, uint32_t image, VulkanSwapchain* oldSwapchain = nullptr);
	VulkanRenderPass CreateRenderPass(VkFormat color, uint32_t count = 1, VkFormat depth = VK_FORMAT_UNDEFINED);
	VulkanFramebuffer CreateFramebuffer(VkImage image, const VkExtent2D& size, const VulkanRenderPass& renderpass);
	VulkanShader CreateShader(const char* binary, size_t length);
	VulkanShader CreateShader(const std::string& filename);
	VulkanBuffer CreateBuffer(VkBufferUsageFlags usage, VkSharingMode sharingMode, VkDeviceSize size);
	VulkanImage CreateImage(VkImageType type, VkFormat format, VkExtent3D size, uint32_t mips, uint32_t arrays, VkSampleCountFlagBits samples, VkImageTiling tiling, VkImageUsageFlags usage);
	VulkanSampler CreateSampler(VkFilter filter, VkSamplerMipmapMode mipmapMode, VkSamplerAddressMode addressMode, float maxAnisotropy);
	VulkanPipelineLayout CreatePipelineLayout(const std::vector<VulkanDescriptorSetLayout>& descriptorSetLayouts);
	VulkanGraphicsPipeline CreateGraphicsPipeline(const VulkanRenderPass& renderpass, 
		const VulkanPipelineLayout& layout, 
		const std::vector<ShaderStageInfo>& stages, 
		VkPrimitiveTopology pt, 
		const VkRect2D& viewport, 
		const std::vector<VkVertexInputBindingDescription>& vertexBindings = std::vector<VkVertexInputBindingDescription>(), 
		const std::vector<VkVertexInputAttributeDescription>& vertexAttributes = std::vector<VkVertexInputAttributeDescription>());
	VulkanDescriptorSetLayout CreateDescriptorSetLayout(std::vector<VkDescriptorSetLayoutBinding> bindings, uint32_t maxSets);

	VkResult WaitForFences(const std::vector<VulkanFence>& fences, bool waitAll = true, uint64_t timeout = 0);
	VkResult ResetFences(const std::vector<VulkanFence>& fences);

	VkResult WaitIdle() const;

	bool operator!() const
	{
		return VK_NULL_HANDLE == f_Device;
	}
private:
	VulkanDevice(VkDevice device, const std::vector<QueueInfo>& queueInfos, VulkanPhysicalDevice& physicalDevice);

	VkPresentModeKHR GetSwapChainPresentMode(const CM::VulkanSurface& surface);
	VkSurfaceFormatKHR GetSwapChainFormat(const CM::VulkanSurface& surface);
	VkExtent2D GetSwapChainImageSize(const VkSurfaceCapabilitiesKHR& scs, uint32_t width, uint32_t height);
	VkImageUsageFlags GetSwapChainUsageFlags(const VkSurfaceCapabilitiesKHR& scs);
	VkSurfaceTransformFlagBitsKHR GetSwapChainTransform(const VkSurfaceCapabilitiesKHR& scs);

	VulkanPhysicalDevice* f_PhysicalDevice;
	VolkDeviceTable f_DeviceFunctions;
	VkDevice f_Device;

	struct QueuePoolInfo
	{
		uint32_t Family;
		uint32_t Count;
		VkCommandPool Pool;
	};

	QueuePoolInfo f_Queues[qtCount];

	int* f_Usage;
};
//------------------------------------------------------------------------------
CM_END
//------------------------------------------------------------------------------
#endif//_CM_VULKAN_DEVICE_h_