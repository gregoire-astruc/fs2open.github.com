
#include "main.h"

#ifdef WIN32
#include <Windows.h>
#endif

#include "Application.h"

#include "include/cef_app.h"

#ifdef WIN32

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrev, LPSTR szCmdLine, int nCmdShow)
{
	CefMainArgs main_args(GetModuleHandle(NULL));

	CefRefPtr<Application> myApplication(new Application());

	// Execute the sub-process logic. This will block until the sub-process should exit.
	return CefExecuteProcess(main_args, myApplication.get());
}

#else

int main(int argc, char* argv[])
{
	CefMainArgs main_args(argc, argv);

	// Execute the sub-process logic. This will block until the sub-process should exit.
	return CefExecuteProcess(main_args, nullptr);
}

#endif