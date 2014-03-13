
#include "main.h"
#include "Application.h"

#include "include/cef_app.h"

#ifdef WIN32

#include <Windows.h>

int WINAPI WinMain(_In_ HINSTANCE hInst, _In_opt_ HINSTANCE hPrev, _In_ LPSTR szCmdLine, _In_ int nCmdShow)
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

	CefRefPtr<Application> myApplication(new Application());

	// Execute the sub-process logic. This will block until the sub-process should exit.
	return CefExecuteProcess(main_args, myApplication.get());
}

#endif