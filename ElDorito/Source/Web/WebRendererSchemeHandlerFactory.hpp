#pragma once
#include <include/cef_scheme.h>
#include <boost/filesystem.hpp>

namespace Anvil::Client::Rendering
{
	class WebRendererSchemeHandlerFactory : public CefSchemeHandlerFactory
	{
		std::string m_Scheme;
		boost::filesystem::path m_Directory;

	public:
		WebRendererSchemeHandlerFactory(const std::string &p_Scheme, const boost::filesystem::path &p_Directory);

	protected:
		CefRefPtr<CefResourceHandler> Create(CefRefPtr<CefBrowser> p_Browser, CefRefPtr<CefFrame> p_Frame, const CefString& p_SchemeName, CefRefPtr<CefRequest> p_Request) override;
		IMPLEMENT_REFCOUNTING(WebRendererSchemeHandlerFactory);
	};
}
