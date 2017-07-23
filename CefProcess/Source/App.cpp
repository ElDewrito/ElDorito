#include "App.hpp"

App::App()
{
	// Create the renderer-side message router
	// These functions must be named the same as the ones defined by ED
	CefMessageRouterConfig config;
	config.js_query_function = "dewQuery";
	config.js_cancel_function = "dewQueryCancel";
	router = CefMessageRouterRendererSide::Create(config);
}

CefRefPtr<CefRenderProcessHandler> App::GetRenderProcessHandler()
{
	return this;
}

void App::OnRegisterCustomSchemes(CefRawPtr<CefSchemeRegistrar> registrar)
{
	registrar->AddCustomScheme("dew", true, false, false, true, true, false);
	registrar->AddCustomScheme("medals", true, false, false, true, true, false);
}

void App::OnContextCreated(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefV8Context> context)
{
	router->OnContextCreated(browser, frame, context);
}

void App::OnContextReleased(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefV8Context> context)
{
	router->OnContextReleased(browser, frame, context);
}

bool App::OnProcessMessageReceived(CefRefPtr<CefBrowser> browser, CefProcessId sourceProcess, CefRefPtr<CefProcessMessage> message)
{
	return router->OnProcessMessageReceived(browser, sourceProcess, message);
}