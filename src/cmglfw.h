/**
	\brief glfw·â×°Àà
*/
//------------------------------------------------------------------------------
#ifndef _CM_GLFW_h_
#define _CM_GLFW_h_ "glfw.h"
//------------------------------------------------------------------------------
#include "cmPublic.h"
#include <functional>
#include <limits>
#if defined(_WIN32)
#include <windows.h>
#if defined(CreateWindow)
#undef CreateWindow
#endif//CreateWindow
#if defined(CreateSemaphore)
#undef CreateSemaphore
#endif//CreateSemaphore
#endif//_WIN32
//------------------------------------------------------------------------------
CM_BEGIN
//------------------------------------------------------------------------------
enum API
{
	apiOpenGL,
	apiVulken,
};
//------------------------------------------------------------------------------
class GLFWwindow;
//------------------------------------------------------------------------------
class GLFW
{
public:
	GLFW(API api);
	virtual ~GLFW();

	GLFWwindow CreateWindow(const char* title, unsigned int width, unsigned int height);

	void WaitEvents(double timeOut = std::numeric_limits<double>::infinity());
	void PollEvents();
private:
	API f_API;
};
//------------------------------------------------------------------------------
class GLFWwindow_i;
//------------------------------------------------------------------------------
class GLFWwindow
{
public:
	friend class GLFW;

	GLFWwindow();
	GLFWwindow(const GLFWwindow& r);
	virtual ~GLFWwindow();

	GLFWwindow& operator=(const GLFWwindow& r);

#if defined(_WIN32)
	HWND GetHWND() const;
#endif//_WIN32

	std::function<void (CM::GLFWwindow*, int, int, int, int)> OnKey;
	std::function<void (CM::GLFWwindow*)> OnRefresh;
	std::function<void (CM::GLFWwindow*, int, int)> OnResize;

	void Show();

	bool ShouldClose();

	bool operator!() const
	{
		return nullptr == f_Impl;
	}
private:
	GLFWwindow(GLFWwindow_i* impl);

	GLFWwindow_i* f_Impl;
	int *f_Usage;
};
//------------------------------------------------------------------------------
CM_END
//------------------------------------------------------------------------------
#endif//_CM_GLFW_h_