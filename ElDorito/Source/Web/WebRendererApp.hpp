#pragma once
#include <include/cef_app.h>

namespace Anvil::Client::Rendering
{
	class WebRendererApp :
		public CefApp,
		public CefBrowserProcessHandler,
		public CefRenderProcessHandler
	{
	public:
		virtual ~WebRendererApp() { }
		virtual void OnRegisterCustomSchemes(CefRawPtr<CefSchemeRegistrar> p_Registrar) override;

		virtual CefRefPtr< CefBrowserProcessHandler > GetBrowserProcessHandler() override;
		virtual CefRefPtr<CefRenderProcessHandler> GetRenderProcessHandler() override;
		virtual void OnContextInitialized() override;
		virtual void OnContextCreated(CefRefPtr< CefBrowser > browser, CefRefPtr< CefFrame > frame, CefRefPtr< CefV8Context > context) override;
		virtual void OnBeforeCommandLineProcessing(const CefString& process_type, CefRefPtr<CefCommandLine> command_line) override;

	protected:
		IMPLEMENT_REFCOUNTING(WebRendererApp);
	};
}
