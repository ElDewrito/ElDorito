/*
Code was used from NoFaTe (http://nofate.me)
*/
#include "WebRendererHandler.hpp"
#include "Logger.hpp"
#include "../Utils/Rectangle.hpp"
#include "Bridge/WebRendererQueryHandler.hpp"
#include "Bridge/Client/ClientFunctions.hpp"

using namespace Anvil::Client::Rendering;

WebRendererHandler::WebRendererHandler(LPDIRECT3DDEVICE9 p_Device) :
	m_Device(p_Device),
	m_Browser(nullptr),
	m_TextureStride(0),
	m_PopupVisible(false),
	m_PopupValid(false)
{
	m_TextureData.resize(0);
	m_PopupData.resize(0);

	// Create the browser-side message router
	// These functions must be named the same as the ones defined by CefProcess
	CefMessageRouterConfig s_RouterConfig;
	s_RouterConfig.js_query_function = "dewQuery";
	s_RouterConfig.js_cancel_function = "dewQueryCancel";
	m_BrowserRouter = CefMessageRouterBrowserSide::Create(s_RouterConfig);
}

void WebRendererHandler::OnAfterCreated(CefRefPtr<CefBrowser> p_Browser)
{
	m_Browser = p_Browser;

	// Register the query handler
	if (!m_QueryHandler)
	{
		m_QueryHandler = std::make_shared<Bridge::WebRendererQueryHandler>();

		m_QueryHandler->AddMethod("hide", Bridge::ClientFunctions::OnHide);
		m_QueryHandler->AddMethod("command", Bridge::ClientFunctions::OnCommand);
		m_QueryHandler->AddMethod("ping", Bridge::ClientFunctions::OnPing);

		m_BrowserRouter->AddHandler(m_QueryHandler.get(), true);
	}
}

void WebRendererHandler::OnBeforeClose(CefRefPtr<CefBrowser> p_Browser)
{
	if (m_BrowserRouter)
		m_BrowserRouter->OnBeforeClose(p_Browser);

	// Release our browser handle
	m_QueryHandler = nullptr;
	m_BrowserRouter = nullptr;
	m_Browser = nullptr;
}

bool WebRendererHandler::GetViewRect(CefRefPtr<CefBrowser> p_Browser, CefRect& p_Rect)
{
	uint32_t s_Width = 0, s_Height = 0;
	if (!GetViewportInformation(s_Width, s_Height))
		return false;

	p_Rect = CefRect(0, 0, s_Width, s_Height);
	return true;
}

bool WebRendererHandler::GetScreenInfo(CefRefPtr<CefBrowser> p_Browser, CefScreenInfo &p_ScreenInfo)
{
	uint32_t s_Width = 0, s_Height = 0;
	if (!GetViewportInformation(s_Width, s_Height))
		return false;
	p_ScreenInfo.device_scale_factor = 1.0f;
	p_ScreenInfo.depth = 32;
	p_ScreenInfo.depth_per_component = 8;
	p_ScreenInfo.is_monochrome = false;
	p_ScreenInfo.rect.x = 0;
	p_ScreenInfo.rect.y = 0;
	p_ScreenInfo.rect.width = s_Width;
	p_ScreenInfo.rect.height = s_Height;
	p_ScreenInfo.available_rect = p_ScreenInfo.rect;
	return true;
}

bool WebRendererHandler::GetScreenPoint(CefRefPtr<CefBrowser> p_Browser, int p_ViewX, int p_ViewY, int &p_ScreenX, int &p_ScreenY)
{
	p_ScreenX = p_ViewX;
	p_ScreenY = p_ViewY;
	return true;
}

void WebRendererHandler::OnPopupShow(CefRefPtr<CefBrowser> p_Browser, bool p_Show)
{
	// Require a redraw when hiding the popup
	m_PopupVisible = p_Show;
	if (!p_Show)
		p_Browser->GetHost()->Invalidate(PET_VIEW);
}

void WebRendererHandler::OnPopupSize(CefRefPtr<CefBrowser> p_Browser, const CefRect &p_Rect)
{
	m_PopupRect = Utils::Rectangle(p_Rect.x, p_Rect.y, p_Rect.width, p_Rect.height);
	m_PopupData.resize(p_Rect.width * p_Rect.height * 4);
	m_PopupValid = false;
}

void WebRendererHandler::OnPaint(CefRefPtr<CefBrowser> p_Browser, PaintElementType p_Type, const RectList& p_DirtyRects, const void* p_Buffer, int p_Width, int p_Height)
{
	if (m_TextureData.size() == 0)
		return;

	m_TextureLock.lock();

	// Copy the dirty rects
	auto s_Stride = p_Width * 4;
	Utils::Rectangle s_PopupDirtyRect;
	for (auto& l_Rect : p_DirtyRects)
	{
		Utils::Rectangle s_SrcRect(l_Rect.x, l_Rect.y, l_Rect.width, l_Rect.height);
		if (p_Type == PET_VIEW)
		{
			// Copy the view data to the screen
			Utils::Rectangle::Copy(m_TextureData.data(), l_Rect.x, l_Rect.y, m_TextureStride, p_Buffer, s_SrcRect, s_Stride, 4);
			m_DirtyRect = m_DirtyRect.Add(s_SrcRect);

			// If the dirty rectangle intersects the popup,
			// then that portion of the popup needs to be updated
			if (m_PopupVisible && m_PopupValid && s_SrcRect.Intersects(m_PopupRect))
				s_PopupDirtyRect = s_PopupDirtyRect.Add(s_SrcRect.Intersect(m_PopupRect));
		}
		else if (p_Type == PET_POPUP)
		{
			// Copy the dirty rectangle to the popup data
			Utils::Rectangle::Copy(m_PopupData.data(), l_Rect.x, l_Rect.y, s_Stride, p_Buffer, s_SrcRect, s_Stride, 4);
			m_PopupValid = true;

			if (m_PopupVisible)
			{
				// ...and to the screen
				auto s_PopupScreenRect = s_SrcRect.Translate(m_PopupRect.X, m_PopupRect.Y);
				Utils::Rectangle::Copy(m_TextureData.data(), s_PopupScreenRect.X, s_PopupScreenRect.Y, m_TextureStride, p_Buffer, s_SrcRect, s_Stride, 4);
				m_DirtyRect = m_DirtyRect.Add(s_PopupScreenRect);
			}
		}
	}

	// If the popup needs to be redrawn, do it
	if (!s_PopupDirtyRect.IsEmpty())
	{
		auto s_PopupStride = m_PopupRect.Width * 4;
		auto s_PopupSrcRect = s_PopupDirtyRect.Translate(-m_PopupRect.X, -m_PopupRect.Y);
		Utils::Rectangle::Copy(m_TextureData.data(), s_PopupDirtyRect.X, s_PopupDirtyRect.Y, m_TextureStride, m_PopupData.data(), s_PopupSrcRect, s_PopupStride, 4);
		// No need to update the dirty rect here
	}

	m_TextureLock.unlock();
}

bool WebRendererHandler::GetViewportInformation(uint32_t& p_Width, uint32_t& p_Height)
{
	if (!m_Device)
		return false;

	D3DVIEWPORT9 s_Viewport;
	auto s_Ret = m_Device->GetViewport(&s_Viewport);
	if (FAILED(s_Ret))
		return false;

	p_Width = s_Viewport.Width;
	p_Height = s_Viewport.Height;

	return true;
}

bool WebRendererHandler::Resize(uint32_t p_Width, uint32_t p_Height)
{
	WriteLog("Resize called: %dx%d.", p_Width, p_Height);
	auto s_TextureDataSize = p_Width * p_Height * 4;
	if (!s_TextureDataSize)
		return false;

	m_TextureLock.lock();

	WriteLog("Tried to allocate 0x%x bytes.", s_TextureDataSize);

	// Resize to our new data
	m_TextureData.resize(s_TextureDataSize);
	m_TextureStride = p_Width * 4;

	// Clear out the buffer
	fill(m_TextureData.begin(), m_TextureData.end(), 0);

	m_TextureLock.unlock();
	return true;
}

uint8_t* WebRendererHandler::GetTexture()
{
	return m_TextureData.data();
}

uint32_t WebRendererHandler::GetTextureLength()
{
	return m_TextureData.size();
}

Utils::Rectangle WebRendererHandler::GetTextureDirtyRect()
{
	return m_DirtyRect;
}

void WebRendererHandler::ResetTextureDirtyRect()
{
	m_DirtyRect = Utils::Rectangle();
}

CefRefPtr<CefBrowser> WebRendererHandler::GetBrowser()
{
	return m_Browser;
}

void WebRendererHandler::LockTexture()
{
	m_TextureLock.lock();
}

void WebRendererHandler::UnlockTexture()
{
	m_TextureLock.unlock();
}

void WebRendererHandler::OnRenderProcessTerminated(CefRefPtr<CefBrowser> p_Browser, TerminationStatus p_Status)
{
	if (m_BrowserRouter)
		m_BrowserRouter->OnRenderProcessTerminated(p_Browser);
}

bool WebRendererHandler::OnBeforeBrowse(CefRefPtr<CefBrowser> p_Browser, CefRefPtr<CefFrame> p_Frame, CefRefPtr<CefRequest> p_Request, bool p_IsRedirect)
{
	if (m_BrowserRouter)
		m_BrowserRouter->OnBeforeBrowse(p_Browser, p_Frame);
	return false;
}

bool WebRendererHandler::OnProcessMessageReceived(CefRefPtr<CefBrowser> p_Browser, CefProcessId p_SourceProcess, CefRefPtr<CefProcessMessage> p_Message)
{
	return m_BrowserRouter->OnProcessMessageReceived(p_Browser, p_SourceProcess, p_Message);
}