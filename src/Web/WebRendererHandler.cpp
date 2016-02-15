/*
Code was used from NoFaTe (http://nofate.me)
*/
#include "WebRendererHandler.hpp"
#include "Logger.hpp"
#include "../Utils/Rectangle.hpp"

using namespace Anvil::Client::Rendering;

WebRendererHandler::WebRendererHandler(LPDIRECT3DDEVICE9 p_Device) :
	m_Device(p_Device),
	m_Browser(nullptr)
{
	m_TextureData.resize(0);
}

void WebRendererHandler::OnAfterCreated(CefRefPtr<CefBrowser> p_Browser)
{
	m_Browser = p_Browser;
}

void WebRendererHandler::OnBeforeClose(CefRefPtr<CefBrowser> p_Browser)
{
	// Release our browser handle
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

void WebRendererHandler::OnPaint(CefRefPtr<CefBrowser> p_Browser, PaintElementType p_Type, const RectList& p_DirtyRects, const void* p_Buffer, int p_Width, int p_Height)
{
	if (m_TextureData.size() == 0)
		return;

	m_TextureLock.lock();

	// Blit the dirty rects
	auto s_Stride = p_Width * 4;
	for (auto& l_Rect : p_DirtyRects)
	{
		Utils::Rectangle s_CopyRect(l_Rect.x, l_Rect.y, l_Rect.width, l_Rect.height);
		Utils::Rectangle::Copy(m_TextureData.data(), l_Rect.x, l_Rect.y, s_Stride, p_Buffer, s_CopyRect, s_Stride, 4);
		m_DirtyRect.Add(s_CopyRect);
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
	m_DirtyRect.Reset();
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