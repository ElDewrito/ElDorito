/*
Code was used from NoFaTe (http://nofate.me)
*/
#include "WebRendererApp.hpp"

using namespace Anvil::Client::Rendering;

void WebRendererApp::OnRegisterCustomSchemes(CefRefPtr<CefSchemeRegistrar> p_Registrar)
{
}

CefRefPtr<CefBrowserProcessHandler> WebRendererApp::GetBrowserProcessHandler()
{
	//WriteLog("GetBrowserProcessHandler.");
	return this;
}

CefRefPtr<CefRenderProcessHandler> WebRendererApp::GetRenderProcessHandler()
{
	//WriteLog("GetRenderProcessHandler.");
	return this;
}

void WebRendererApp::OnContextInitialized()
{
	//WriteLog("OnContextInitialized.");
}

void WebRendererApp::OnContextCreated(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefV8Context> context)
{
	//WriteLog("OnContextCreated.");
}

void WebRendererApp::OnBeforeCommandLineProcessing(const CefString& process_type, CefRefPtr<CefCommandLine> command_line)
{
	// CEF will NOT shut down properly unless this switch is passed
	// See https://bitbucket.org/chromiumembedded/cef/issues/1680/windows-2454-crash-on-shutdown-with-multi
	command_line->AppendSwitch("disable-extensions");
}