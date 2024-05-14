#pragma once

#ifdef WL_PLATFORM_WINDOWS

extern RayTracing::Application* RayTracing::CreateApplication(int argc, char** argv);
bool g_ApplicationRunning = true;

namespace RayTracing {

	int Main(int argc, char** argv)
	{
		while (g_ApplicationRunning)
		{
			RayTracing::Application* app = RayTracing::CreateApplication(argc, argv);
			app->Run();
			delete app;
		}

		return 0;
	}

}

#ifdef WL_DIST

#include <Windows.h>

int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hInstPrev, PSTR cmdline, int cmdshow)
{
	return RayTracing::Main(__argc, __argv);
}

#else

int main(int argc, char** argv)
{
	return RayTracing::Main(argc, argv);
}

#endif // WL_DIST

#endif // WL_PLATFORM_WINDOWS
