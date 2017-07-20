/*
Code was used from NoFaTe (http://nofate.me)
*/
#include "WebRendererApp.hpp"

using namespace Anvil::Client::Rendering;

void WebRendererApp::OnRegisterCustomSchemes(CefRawPtr<CefSchemeRegistrar> p_Registrar)
{
	p_Registrar->AddCustomScheme("dew", true, false, false, true, true, false);
	p_Registrar->AddCustomScheme("medals", true, false, false, true, true, false);
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

	// These switches make performance significantly faster, especially at high resolutions
	// See https://bitbucket.org/chromiumembedded/cef/commits/e3c1d8632eb43c1c2793d71639f3f5695696a5e8
	command_line->AppendSwitch("disable-gpu");
	command_line->AppendSwitch("disable-gpu-compositing");
	command_line->AppendSwitch("enable-begin-frame-scheduling");
	command_line->AppendSwitch("enable-experimental-web-platform-features");
	command_line->AppendSwitch("enable-media-stream");
	command_line->AppendSwitch("enable-precise-memory-info");
}