/**
	\brief Vulkan着色器封装类
*/
//------------------------------------------------------------------------------
#ifndef _CM_VULKAN_SHADER_h_
#define _CM_VULKAN_SHADER_h_ "cmVulkanShader.h"
//------------------------------------------------------------------------------
#include "cmPublic.h"
#include "cmGLFWVulkan.h"
#include <vector>
//------------------------------------------------------------------------------
CM_BEGIN
//------------------------------------------------------------------------------
class VulkanDevice;
//------------------------------------------------------------------------------
class VulkanShader
{
public:
	friend class VulkanDevice;

	VulkanShader();
	VulkanShader(VulkanShader& r);
	~VulkanShader();

	VulkanShader& operator=(const VulkanShader& r);
	VulkanShader& operator=(VulkanShader&& r);

	bool operator!() const
	{
		return VK_NULL_HANDLE == f_Shader;
	}
private:
	VulkanShader(VkShaderModule shader, VulkanDevice* device);

	VulkanDevice* f_Device;
	VkShaderModule f_Shader;

	int* f_Usage;
};
//------------------------------------------------------------------------------
CM_END
//------------------------------------------------------------------------------
#endif//_CM_VULKAN_SHADER_h_