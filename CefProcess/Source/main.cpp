#include <Windows.h>
#include <include/cef_app.h>
#include "App.hpp"

// Disable warnings about missing PDB files
#pragma warning(disable:4099)

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	// Enable High-DPI support on Windows 7 or newer.
	CefEnableHighDPISupport();

	// Provide CEF with command-line arguments.
	CefMainArgs args(hInstance);

	// Set up the app instance.
	CefRefPtr<App> app(new App());

	// CEF applications have multiple sub-processes (render, plugin, GPU, etc)
	// that share the same executable. This function checks the command-line and,
	// if this is a sub-process, executes the appropriate logic.
	auto exitCode = CefExecuteProcess(args, app.get(), nullptr);
	if (exitCode >= 0) {
		// The sub-process has completed so return here.
		return exitCode;
	}

	MessageBox(nullptr, "Must be run by mtndew.dll!", "ElDewrito CEF Process", MB_OK | MB_ICONERROR);
	return 0;
}
