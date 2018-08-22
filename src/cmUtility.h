/**
	\brief Õ®”√¿‡
*/
//------------------------------------------------------------------------------
#ifndef _CM_UTILITY_h_
#define _CM_UTILITY_h_ "cmUtility.h"
//------------------------------------------------------------------------------
#include "cmPublic.h"
#include "cmGLFWVulkan.h"
#include <vector>
#include <algorithm>
//------------------------------------------------------------------------------
CM_BEGIN
//------------------------------------------------------------------------------
inline bool CheckExtensionSupport(const std::vector<VkExtensionProperties>& eps, const std::vector<const char*>& es)
{
	for (auto e : es)
	{
		if (!std::any_of(begin(eps), end(eps), [e](const VkExtensionProperties& ep)->bool{ return 0 == strcmp(e, ep.extensionName); }))
		{
			return false;
		}
	}
	return true;
}
//------------------------------------------------------------------------------
template<typename UnaryPredicate>
uint32_t FindQueue(const std::vector<VkQueueFamilyProperties>& qfps, UnaryPredicate p)
{
	for (uint32_t qf = 0; qf < qfps.size(); ++qf)
	{
		auto qfp = qfps[qf];
		if (qfp.queueCount > 0 && p(qfp))
		{
			return qf;
		}
	}
	return (uint32_t)-1;
};
//------------------------------------------------------------------------------
std::vector<uint8_t> LoadImage(const char* filename, uint32_t& width, uint32_t& height, uint32_t& component);
//------------------------------------------------------------------------------
CM_END
//------------------------------------------------------------------------------
#endif//_CM_UTILITY_h_