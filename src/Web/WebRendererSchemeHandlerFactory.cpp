/*
	Code was used from NoFaTe (http://nofate.me)
*/
#include "WebRendererSchemeHandlerFactory.hpp"
#include "WebRendererSchemeHandler.hpp"

using namespace Anvil::Client::Rendering;

WebRendererSchemeHandlerFactory::WebRendererSchemeHandlerFactory(const std::string &p_Scheme, const boost::filesystem::path &p_Directory)
	: m_Scheme(p_Scheme), m_Directory(p_Directory)
{
}


CefRefPtr<CefResourceHandler> WebRendererSchemeHandlerFactory::Create(CefRefPtr<CefBrowser> p_Browser, CefRefPtr<CefFrame> p_Frame, const CefString& p_SchemeName, CefRefPtr<CefRequest> p_Request)
{
	return new WebRendererSchemeHandler(m_Scheme, m_Directory, p_Frame->IsMain(), p_Frame);
}