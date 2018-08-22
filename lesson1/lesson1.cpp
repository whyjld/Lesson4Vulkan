// lesson1.cpp : 枚举设备信息。
//

#include "stdafx.h"
#include "../src/cmglfw.h"
#include "../src/cmvulkan.h"
#include <algorithm>

#define OUTPUT_PROPERTY(o, x) std::cout << "\t"#x << ":\t" << o##.##x << std::endl;
#define OUTPUT_VERSION(o, x) std::cout << "\t"#x << ":\t" << VK_VERSION_MAJOR(o##.##x) << "." << VK_VERSION_MINOR(o##.##x) << "." << VK_VERSION_PATCH(o##.##x) << std::endl;
#define OUTPUT_BOOL(o, x)  std::cout << "\t"#x << ":\t" << ((o##.##x != 0) ? "true" : "false") << std::endl; 
#define OUTPUT_ENUM(o, x, e) if(e == o##.##x) std::cout << "\t"#x << ":\t" << #e << std::endl; 
#define OUTPUT_BIT(o, x, b) if(0 != (b & o##.##x)) std::cout << "\t"#x << ":\t" << #b << std::endl; 

void OutputDeviceProperties(CM::VulkanPhysicalDevice& pd)
{
	auto p = pd.GetProperties();

	std::cout << "\tProperties:" << std::endl;
	OUTPUT_VERSION(p, apiVersion);
	OUTPUT_VERSION(p, driverVersion);
	OUTPUT_PROPERTY(p, vendorID);
	OUTPUT_PROPERTY(p, deviceID);
	OUTPUT_ENUM(p, deviceType, VK_PHYSICAL_DEVICE_TYPE_OTHER);
	OUTPUT_ENUM(p, deviceType, VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU);
	OUTPUT_ENUM(p, deviceType, VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU);
	OUTPUT_ENUM(p, deviceType, VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU);
	OUTPUT_ENUM(p, deviceType, VK_PHYSICAL_DEVICE_TYPE_CPU);
	OUTPUT_PROPERTY(p, deviceName);
	OUTPUT_PROPERTY(p, pipelineCacheUUID);

	std::cout << std::endl;
}

void OutputDeviceFeatures(CM::VulkanPhysicalDevice& pd)
{
	auto f = pd.GetFeatures();

	std::cout << "\tFeatures:" << std::endl;
	OUTPUT_BOOL(f, robustBufferAccess);
	OUTPUT_BOOL(f, fullDrawIndexUint32);
	OUTPUT_BOOL(f, imageCubeArray);
	OUTPUT_BOOL(f, independentBlend);
	OUTPUT_BOOL(f, geometryShader);
	OUTPUT_BOOL(f, tessellationShader);
	OUTPUT_BOOL(f, sampleRateShading);
	OUTPUT_BOOL(f, dualSrcBlend);
	OUTPUT_BOOL(f, logicOp);
	OUTPUT_BOOL(f, multiDrawIndirect);
	OUTPUT_BOOL(f, drawIndirectFirstInstance);
	OUTPUT_BOOL(f, depthClamp);
	OUTPUT_BOOL(f, depthBiasClamp);
	OUTPUT_BOOL(f, fillModeNonSolid);
	OUTPUT_BOOL(f, depthBounds);
	OUTPUT_BOOL(f, wideLines);
	OUTPUT_BOOL(f, largePoints);
	OUTPUT_BOOL(f, alphaToOne);
	OUTPUT_BOOL(f, multiViewport);
	OUTPUT_BOOL(f, samplerAnisotropy);
	OUTPUT_BOOL(f, textureCompressionETC2);
	OUTPUT_BOOL(f, textureCompressionASTC_LDR);
	OUTPUT_BOOL(f, textureCompressionBC);
	OUTPUT_BOOL(f, occlusionQueryPrecise);
	OUTPUT_BOOL(f, pipelineStatisticsQuery);
	OUTPUT_BOOL(f, vertexPipelineStoresAndAtomics);
	OUTPUT_BOOL(f, fragmentStoresAndAtomics);
	OUTPUT_BOOL(f, shaderTessellationAndGeometryPointSize);
	OUTPUT_BOOL(f, shaderImageGatherExtended);
	OUTPUT_BOOL(f, shaderStorageImageExtendedFormats);
	OUTPUT_BOOL(f, shaderStorageImageMultisample);
	OUTPUT_BOOL(f, shaderStorageImageReadWithoutFormat);
	OUTPUT_BOOL(f, shaderStorageImageWriteWithoutFormat);
	OUTPUT_BOOL(f, shaderUniformBufferArrayDynamicIndexing);
	OUTPUT_BOOL(f, shaderSampledImageArrayDynamicIndexing);
	OUTPUT_BOOL(f, shaderStorageBufferArrayDynamicIndexing);
	OUTPUT_BOOL(f, shaderStorageImageArrayDynamicIndexing);
	OUTPUT_BOOL(f, shaderClipDistance);
	OUTPUT_BOOL(f, shaderCullDistance);
	OUTPUT_BOOL(f, shaderFloat64);
	OUTPUT_BOOL(f, shaderInt64);
	OUTPUT_BOOL(f, shaderInt16);
	OUTPUT_BOOL(f, shaderResourceResidency);
	OUTPUT_BOOL(f, shaderResourceMinLod);
	OUTPUT_BOOL(f, sparseBinding);
	OUTPUT_BOOL(f, sparseResidencyBuffer);
	OUTPUT_BOOL(f, sparseResidencyImage2D);
	OUTPUT_BOOL(f, sparseResidencyImage3D);
	OUTPUT_BOOL(f, sparseResidency2Samples);
	OUTPUT_BOOL(f, sparseResidency4Samples);
	OUTPUT_BOOL(f, sparseResidency8Samples);
	OUTPUT_BOOL(f, sparseResidency16Samples);
	OUTPUT_BOOL(f, sparseResidencyAliased);
	OUTPUT_BOOL(f, variableMultisampleRate);
	OUTPUT_BOOL(f, inheritedQueries);

	std::cout << std::endl;
}

void OutputMemoryProperties(CM::VulkanPhysicalDevice& pd)
{
	auto mp = pd.GetMemoryProperties();
	std::cout << "\tMemory properties count:" << mp.memoryTypeCount << std::endl;
	for (uint32_t m = 0; m < mp.memoryTypeCount; ++m)
	{
		std::cout << "\tMemory type " << m << "/" << mp.memoryTypeCount << std::endl;

		auto t = mp.memoryTypes[m];
		OUTPUT_PROPERTY(t, heapIndex);
		OUTPUT_BIT(t, propertyFlags, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
		OUTPUT_BIT(t, propertyFlags, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
		OUTPUT_BIT(t, propertyFlags, VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
		OUTPUT_BIT(t, propertyFlags, VK_MEMORY_PROPERTY_HOST_CACHED_BIT);
		OUTPUT_BIT(t, propertyFlags, VK_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT);

		if (t.heapIndex < mp.memoryHeapCount)
		{
			auto mht = mp.memoryHeaps[t.heapIndex];
			if (0 != (mht.flags & VK_MEMORY_HEAP_DEVICE_LOCAL_BIT))
			{
				std::cout << "\tDevice local memory." << std::endl;
			}
			std::vector<uint64_t> ss;
			uint64_t s = mht.size;
			for (; s > 0 && ss.size() < 4; s /= 1024)
			{
				ss.push_back(s % 1024);
			}
			if (s > 0)
			{
				ss.push_back(s);
			}
			const char sd[] = "BKMGT";

			std::cout << "\tMemory size:";
			if (ss.size() > 0)
			{
				const char* ssd = sd + 5 - ss.size();
				for (size_t i = ss.size(); i > 0; --i)
				{
					if (ss[i - 1] > 0)
					{
						std::cout << ss[i - 1] << sd[i - 1];
					}
				}
				std::cout << std::endl;
			}
			else
			{
				std::cout << "0B" << std::endl;
			}
		}
		else
		{
			std::cout << "\tOut of memory heap index range." << std::endl;
		}

		std::cout << std::endl;
	}
}

void OutputQueueFamilyProperties(CM::VulkanPhysicalDevice& pd)
{
	auto qfp = pd.GetQueueFamilyProperties();
	std::cout << "\tQueue family count:" << qfp.size() << std::endl;
	for (uint32_t q = 0; q < qfp.size(); ++q)
	{
		std::cout << "\tQueue family " << q << "/" << qfp.size() << std::endl;
		OUTPUT_PROPERTY(qfp[q], queueCount);
		OUTPUT_BIT(qfp[q], queueFlags, VK_QUEUE_GRAPHICS_BIT);
		OUTPUT_BIT(qfp[q], queueFlags, VK_QUEUE_COMPUTE_BIT);
		OUTPUT_BIT(qfp[q], queueFlags, VK_QUEUE_TRANSFER_BIT);
		OUTPUT_BIT(qfp[q], queueFlags, VK_QUEUE_SPARSE_BINDING_BIT);
		OUTPUT_PROPERTY(qfp[q], timestampValidBits);
		OUTPUT_PROPERTY(qfp[q], minImageTransferGranularity.width);
		OUTPUT_PROPERTY(qfp[q], minImageTransferGranularity.height);
		OUTPUT_PROPERTY(qfp[q], minImageTransferGranularity.depth);

		std::cout << std::endl;
	}
}

int _tmain(int argc, _TCHAR* argv[])
{
	CM::GLFW glfw(CM::apiVulken);

	CM::VulkanInstance vi("lesson1", "demo");

	unsigned int pdc = vi.GetPhysicalDeviceCount();
	std::cout << "Device count:" << pdc << std::endl;
	for (unsigned int i = 0; i < pdc; ++i)
	{
		std::cout << "Device " << i << "/" << pdc << std::endl;
		auto pd = vi.GetPhysicalDevice(i);
		
		OutputDeviceProperties(pd);
		OutputDeviceFeatures(pd);
		OutputMemoryProperties(pd);
		OutputQueueFamilyProperties(pd);
	}

	return 0;
}

