/**
	\brief 公共定义
*/
//------------------------------------------------------------------------------
#ifndef _CM_PUBLIC_h_
#define _CM_PUBLIC_h_ "cmPublic.h"
//------------------------------------------------------------------------------
#include <exception>
#include <iostream>
#include <cstdint>
#include <vector>
//------------------------------------------------------------------------------
#define CM_BEGIN namespace CM \
{
//------------------------------------------------------------------------------
#define CM_END };
//------------------------------------------------------------------------------
typedef std::exception cmException;
//------------------------------------------------------------------------------
class cmSuboptimal : public cmException
{
public:
	cmSuboptimal(const char * const & msg)
		: cmException(msg)
	{

	}
};
//------------------------------------------------------------------------------
class cmErrorOutOfDate : public cmException
{
public:
	cmErrorOutOfDate(const char * const & msg)
		: cmException(msg)
	{

	}
};
//------------------------------------------------------------------------------
#define THROW(x) { \
	std::cout << x << std::endl; \
	throw cmException(x); \
}
//------------------------------------------------------------------------------
CM_BEGIN
//------------------------------------------------------------------------------
enum QueueType
{
	qtGraphics,
	qtCompute,
	qtPresent,
	qtCount,
};
//------------------------------------------------------------------------------
struct QueueInfo
{
	uint32_t Family;
	std::vector<float> Priorities;
	std::vector<QueueType> Types;
};
//------------------------------------------------------------------------------
CM_END
//------------------------------------------------------------------------------
#endif//_CM_PUBLIC_h_