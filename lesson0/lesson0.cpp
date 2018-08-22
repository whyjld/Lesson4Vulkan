// lesson0.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "../src/cmglfw.h"
#include <algorithm>

int _tmain(int argc, _TCHAR* argv[])
{
	CM::GLFW glfw(CM::apiVulken);
	CM::GLFWwindow win = glfw.CreateWindow("main", 100, 100);
	win.Show();

	for (; !win.ShouldClose();)
	{
		glfw.WaitEvents();
	}

	return 0;
}

