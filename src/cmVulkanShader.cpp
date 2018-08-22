/**
	\brief Vulkan 着色器封装类
*/
//------------------------------------------------------------------------------
#include "cmVulkanDevice.h"
#include "cmVulkanShader.h"
//------------------------------------------------------------------------------
CM_BEGIN
//------------------------------------------------------------------------------
VulkanShader::VulkanShader()
: f_Device(nullptr), f_Shader(VK_NULL_HANDLE), f_Usage(new int(1))
{

}
//------------------------------------------------------------------------------
VulkanShader::VulkanShader(VkSemaphore semaphore, VulkanDevice* device)
	: f_Device(device), f_Shader(semaphore), f_Usage(new int(1))
{
}
//------------------------------------------------------------------------------
VulkanShader::VulkanShader(VulkanShader& r)
	: f_Device(r.f_Device), f_Shader(r.f_Shader), f_Usage(r.f_Usage)
{
	++(*f_Usage);
}
//------------------------------------------------------------------------------
VulkanShader::~VulkanShader()
{
	if (0 == --(*f_Usage))
	{
		if (VK_NULL_HANDLE != f_Shader)
		{
			f_Device->f_DeviceFunctions.vkDestroyShaderModule(f_Device->f_Device, f_Shader, nullptr);
		}
		delete f_Usage;
	}
}
//------------------------------------------------------------------------------
VulkanShader& VulkanShader::operator=(const VulkanShader& r)
{
	if (this != &r)
	{
		if (0 == --(*f_Usage))
		{
			if (VK_NULL_HANDLE != f_Shader)
			{
				f_Device->f_DeviceFunctions.vkDestroyShaderModule(f_Device->f_Device, f_Shader, nullptr);
			}
			delete f_Usage;
		}
		f_Device = r.f_Device;
		f_Shader = r.f_Shader;
		f_Usage = r.f_Usage;
		++(*f_Usage);
	}
	return *this;
}
//------------------------------------------------------------------------------
VulkanShader& VulkanShader::operator=(VulkanShader&& r)
{
	if (this != &r)
	{
		std::swap(f_Device, r.f_Device);
		std::swap(f_Shader, r.f_Shader);
		std::swap(f_Usage, r.f_Usage);
	}
	return *this;
}
//------------------------------------------------------------------------------
CM_END
