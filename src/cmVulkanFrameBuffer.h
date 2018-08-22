/**
	\brief VulkanÖ¡»º³å·â×°Àà
*/
//------------------------------------------------------------------------------
#ifndef _CM_VULKAN_FRAMEBUFFER_h_
#define _CM_VULKAN_FRAMEBUFFER_h_ "cmVulkanFramebuffer.h"
//------------------------------------------------------------------------------
#include "cmPublic.h"
#include "cmGLFWVulkan.h"
#include <vector>
//------------------------------------------------------------------------------
CM_BEGIN
//------------------------------------------------------------------------------
class VulkanDevice;
//------------------------------------------------------------------------------
class VulkanFramebuffer
{
public:
	friend class VulkanDevice;
	friend class VulkanCommandBuffer;

	VulkanFramebuffer();
	VulkanFramebuffer(VulkanFramebuffer& r);
	~VulkanFramebuffer();

	VulkanFramebuffer& operator=(const VulkanFramebuffer& r);
	VulkanFramebuffer& operator=(VulkanFramebuffer&& r);

	bool operator!() const
	{
		return VK_NULL_HANDLE == f_Framebuffer;
	}
private:
	VulkanFramebuffer(VkFramebuffer framebuffer, VulkanDevice* device);

	VulkanDevice* f_Device;
	VkFramebuffer f_Framebuffer;

	int* f_Usage;
};
//------------------------------------------------------------------------------
CM_END
//------------------------------------------------------------------------------
#endif//_CM_VULKAN_FRAMEBUFFER_h_