#pragma once
#include <include/cef_scheme.h>

namespace Anvil
{
	namespace Client
	{
		namespace Rendering
		{
			class WebRendererSchemeHandlerFactory : public CefSchemeHandlerFactory
			{
			protected:
				CefRefPtr<CefResourceHandler> Create(CefRefPtr<CefBrowser> p_Browser, CefRefPtr<CefFrame> p_Frame, const CefString& p_SchemeName, CefRefPtr<CefRequest> p_Request) override;
				IMPLEMENT_REFCOUNTING(WebRendererSchemeHandlerFactory);
			};
		}
	}
}