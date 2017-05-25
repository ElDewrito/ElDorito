#pragma once
#include <include/cef_app.h>
#include <include/wrapper/cef_message_router.h>

class App :
	public CefApp,
	public CefRenderProcessHandler
{
	CefRefPtr<CefMessageRouterRendererSide> router;

public:
	App();
	virtual void OnRegisterCustomSchemes(CefRawPtr<CefSchemeRegistrar> registrar) override;
	CefRefPtr<CefRenderProcessHandler> GetRenderProcessHandler() override;

	void OnContextCreated(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefV8Context> context) override;
	void OnContextReleased(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefV8Context> context) override;
	bool OnProcessMessageReceived(CefRefPtr<CefBrowser> browser, CefProcessId sourceProcess, CefRefPtr<CefProcessMessage> message) override;

private:
	IMPLEMENT_REFCOUNTING(App);
};