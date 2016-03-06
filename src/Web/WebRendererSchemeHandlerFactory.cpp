/*
	Code was used from NoFaTe (http://nofate.me)
*/
#include "WebRendererSchemeHandlerFactory.hpp"
#include "WebRendererSchemeHandler.hpp"

using namespace Anvil::Client::Rendering;

CefRefPtr<CefResourceHandler> WebRendererSchemeHandlerFactory::Create(CefRefPtr<CefBrowser> p_Browser, CefRefPtr<CefFrame> p_Frame, const CefString& p_SchemeName, CefRefPtr<CefRequest> p_Request)
{
	return new WebRendererSchemeHandler(p_Frame->IsMain(), p_Frame);
}