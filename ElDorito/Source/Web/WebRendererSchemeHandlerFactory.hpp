#pragma once
#include <include/cef_scheme.h>

namespace Anvil::Client::Rendering
{
	// The methods of this class will always be called on the IO thread.
	class WebRendererSchemeHandlerFactory : public CefSchemeHandlerFactory
	{
		const std::string m_Directory;
		const std::string m_Scheme;

	public:
		WebRendererSchemeHandlerFactory(const std::string &p_Scheme, const std::string &p_Directory);

	protected:
		CefRefPtr<CefResourceHandler> Create(CefRefPtr<CefBrowser> p_Browser, CefRefPtr<CefFrame> p_Frame, const CefString& p_SchemeName, CefRefPtr<CefRequest> p_Request) override;
		IMPLEMENT_REFCOUNTING(WebRendererSchemeHandlerFactory);
	};
}
