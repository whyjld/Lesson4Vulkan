/**
	\brief GLFW·â×°Àà
*/
//------------------------------------------------------------------------------
#include "cmglfw.h"
#include <GLFW/glfw3.h>
#if defined(_WIN32)
#define GLFW_EXPOSE_NATIVE_WIN32 1
#include <GLFW/glfw3native.h>
#endif//_WIN32
#include <algorithm>
#include <vector>
//------------------------------------------------------------------------------
#pragma comment(lib, "glfw3.lib")
//------------------------------------------------------------------------------
void error_callback(int error, const char* description)
{
	std::cout << "Error(" << error << "):" << description << std::endl;
}
//------------------------------------------------------------------------------
CM_BEGIN
//------------------------------------------------------------------------------
class GLFWwindow_i
{
public:
	GLFWwindow_i(::GLFWwindow* window)
		: f_Window(window)
	{
		glfwSetWindowUserPointer(f_Window, this);
		glfwSetKeyCallback(f_Window, GLFWwindow_i::KeyCallback);
		glfwSetWindowRefreshCallback(f_Window, GLFWwindow_i::RefreshCallback);
		glfwSetFramebufferSizeCallback(f_Window, GLFWwindow_i::ResizeCallback);
	}

	~GLFWwindow_i()
	{

	}

#if defined(_WIN32)
	HWND GetHWND() const
	{
		return glfwGetWin32Window(f_Window);
	}
#endif//_WIN32

	void Show()
	{
		glfwShowWindow(f_Window);
	}

	bool ShouldClose()
	{
		return glfwWindowShouldClose(f_Window) != 0;
	}

	void OnKey(int key, int scancode, int action, int mods)
	{
		std::for_each(f_Interfaces.begin(), f_Interfaces.end(), [key, scancode, action, mods](GLFWwindow* i)
		{
			if (!!i->OnKey)
			{
				i->OnKey(i, key, scancode, action, mods);
			}
		});
	}
	void OnRefresh()
	{
		std::for_each(f_Interfaces.begin(), f_Interfaces.end(), [](GLFWwindow* i)
		{
			if (!!i->OnRefresh)
			{
				i->OnRefresh(i);
			}
		});
	}
	void OnResize(int width, int height)
	{
		std::for_each(f_Interfaces.begin(), f_Interfaces.end(), [width, height](GLFWwindow* i)
		{
			if (!!i->OnResize)
			{
				i->OnResize(i, width, height);
			}
		});
	}

	void AddInterface(GLFWwindow* inf)
	{
		f_Interfaces.push_back(inf);
	}

	void EraseInterface(GLFWwindow* inf)
	{
		for (auto i = f_Interfaces.begin(); i != f_Interfaces.end(); ++i)
		{
			if (*i == inf)
			{
				f_Interfaces.erase(i);
				break;
			}
		}
	}
private:
	static void KeyCallback(::GLFWwindow* window, int key, int scancode, int action, int mods)
	{
		((GLFWwindow_i*)glfwGetWindowUserPointer(window))->OnKey(key, scancode, action, mods);
	}
	static void RefreshCallback(::GLFWwindow* window)
	{
		((GLFWwindow_i*)glfwGetWindowUserPointer(window))->OnRefresh();
	}
	static void ResizeCallback(::GLFWwindow* window, int width, int height)
	{
		((GLFWwindow_i*)glfwGetWindowUserPointer(window))->OnResize(width, height);
	}

	::GLFWwindow* f_Window;
	std::vector<GLFWwindow*> f_Interfaces;
};
//------------------------------------------------------------------------------
GLFW::GLFW(API api)
	: f_API(api)
{
	glfwSetErrorCallback(error_callback);

	if (!glfwInit())
	{
		THROW("Cannot initialize GLFW.");
		exit(1);
	}

	if (apiVulken == f_API)
	{
		if (!glfwVulkanSupported())
		{
			THROW("GLFW failed to find the Vulkan loader.");
		}
	}
}
//------------------------------------------------------------------------------
GLFW::~GLFW()
{
	glfwTerminate();
}
//------------------------------------------------------------------------------
GLFWwindow GLFW::CreateWindow(const char* title, unsigned int width, unsigned int height)
{
	//glfwWindowHint(GLFW_DECORATED, false);
	glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
	if (apiVulken == f_API)
	{
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	}

	::GLFWwindow* window = glfwCreateWindow(width, height, title, NULL, NULL);
	if (nullptr == window)
	{
		THROW("Can't create window");
	}
	return GLFWwindow(new GLFWwindow_i(window));
}
//------------------------------------------------------------------------------
void GLFW::WaitEvents(double timeOut)
{
	if (std::numeric_limits<double>::infinity() == timeOut)
	{
		glfwWaitEvents();
	}
	else
	{
		glfwWaitEventsTimeout(timeOut);
	}
}
//------------------------------------------------------------------------------
void GLFW::PollEvents()
{
	glfwPollEvents();
}
//------------------------------------------------------------------------------
GLFWwindow::GLFWwindow()
	: f_Impl(nullptr), f_Usage(new int(1))
{

}
//------------------------------------------------------------------------------
GLFWwindow::GLFWwindow(GLFWwindow_i* impl)
	:f_Impl(impl), f_Usage(new int(1))
{
	f_Impl->AddInterface(this);
}
//------------------------------------------------------------------------------
GLFWwindow::GLFWwindow(const GLFWwindow& r)
	: f_Impl(r.f_Impl), f_Usage(r.f_Usage)
{
	++(*f_Usage);

	if (nullptr != f_Impl)
	{
		f_Impl->AddInterface(this);
	}
}
//------------------------------------------------------------------------------
GLFWwindow::~GLFWwindow()
{
	if (nullptr != f_Impl)
	{
		f_Impl->EraseInterface(this);
	}

	if (0 == (--(*f_Usage)))
	{
		delete f_Impl;
		delete f_Usage;
	}
}
//------------------------------------------------------------------------------
GLFWwindow& GLFWwindow::operator=(const GLFWwindow& r)
{
	if (this != &r)
	{
		if (nullptr != f_Impl)
		{
			f_Impl->EraseInterface(this);
		}

		if (0 == (--(*f_Usage)))
		{
			delete f_Impl;
			delete f_Usage;
		}

		f_Impl = r.f_Impl;
		f_Usage = r.f_Usage;
		++(*f_Usage);

		if (nullptr != f_Impl)
		{
			f_Impl->AddInterface(this);
		}
	}
	return *this;
}
//------------------------------------------------------------------------------
#if defined(_WIN32)
HWND GLFWwindow::GetHWND() const
{
	return f_Impl->GetHWND();
}
#endif//_WIN32
//------------------------------------------------------------------------------
void GLFWwindow::Show()
{
	f_Impl->Show();
}
//------------------------------------------------------------------------------
bool GLFWwindow::ShouldClose()
{
	return f_Impl->ShouldClose();
}
//------------------------------------------------------------------------------
CM_END