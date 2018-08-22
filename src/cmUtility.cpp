/**
	\brief Õ®”√¿‡
*/
//------------------------------------------------------------------------------
#include "cmPublic.h"
#include "cmUtility.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <fstream>
#include <algorithm>
//------------------------------------------------------------------------------
CM_BEGIN
//------------------------------------------------------------------------------
std::vector<uint8_t> LoadImage(const char* filename, uint32_t& width, uint32_t& height, uint32_t& component)
{
	std::vector<uint8_t> buf;

	if (true)
	{
		std::ifstream inf(filename, std::ios::binary | std::ios::in);
		if (!inf)
		{
			return buf;
		}
		inf.seekg(0, std::ios::end);
		buf.resize(static_cast<size_t>(inf.tellg()));
		inf.seekg(0, std::ios::beg);
		inf.read(reinterpret_cast<char*>(&buf[0]), buf.size());
	}

	int tw = 0, th = 0, tc = 0;
	unsigned char *image = stbi_load_from_memory(reinterpret_cast<unsigned char*>(&buf[0]), static_cast<int>(buf.size()), &tw, &th, &tc, component);
	if ((nullptr == image) || (tw <= 0) || (th <= 0) || (tc <= 0))
	{
		return std::vector<uint8_t>();
	}

	buf.resize(tw * th * ((0 == component) ? tc : component));
	memcpy(&buf[0], image, buf.size());
	width = tw;
	height = th;
	component = tc;

	stbi_image_free(image);

	return buf;
}
//------------------------------------------------------------------------------
CM_END
