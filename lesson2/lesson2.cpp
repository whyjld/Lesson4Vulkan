// lesson2.cpp : Ã¶¾ÙÀ©Õ¹¡£
//

#include "stdafx.h"
#include "../src/cmglfw.h"
#include "../src/cmvulkan.h"
#include <algorithm>

int _tmain(int argc, _TCHAR* argv[])
{
	CM::GLFW glfw(CM::apiVulken);

	CM::VulkanInstance vi("lesson2", "demo");

	auto exts = CM::VulkanInstance::GetExtensionProperties();
	std::cout << "Instance level extensions count:" << exts.size() << std::endl;

	for (size_t i = 0; i < exts.size(); ++i)
	{
		auto ext = exts[i];
		std::cout << "\t" << i << "/" << exts.size() << "\t"
			<< "ExtensionName(Version):" << ext.extensionName << "(" << ext.specVersion << ")" << std::endl;
	}

	unsigned int pdc = vi.GetPhysicalDeviceCount();
	std::cout << "Device count:" << pdc << std::endl;

	for (unsigned int i = 0; i < pdc; ++i)
	{
		std::cout << "\tDevice " << i << "/" << pdc << std::endl;

		auto exts = vi.GetPhysicalDevice(i).GetExtensionProperties();
		std::cout << "\tDevice level extensions count:" << exts.size() << std::endl;
		for (unsigned int e = 0; e < exts.size(); ++e)
		{
			auto ext = exts[e];
			std::cout << "\t" << e << "/" << exts.size() << "\t"
				<< "\tExtensionName(Version):" << ext.extensionName << "(" << ext.specVersion << ")" << std::endl;
		}
	}
	return 0;
}
