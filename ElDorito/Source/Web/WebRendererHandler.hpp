#pragma once
#include <include/cef_client.h>
#include <include/cef_render_handler.h>
#include <include/wrapper/cef_message_router.h>
#include <d3d9.h>
#include <mutex>
#include <memory>
#include "../Utils/Rectangle.hpp"

namespace Anvil::Client::Rendering
{
	namespace Bridge
	{
		class WebRendererQueryHandler;
	}

	class WebRendererHandler :
		public CefClient,
		public CefLifeSpanHandler,
		public CefRenderHandler,
		public CefRequestHandler,
		public CefContextMenuHandler
	{
		LPDIRECT3DDEVICE9 m_Device;
		HWND m_Window;
		std::mutex m_TextureLock;
		std::vector<uint8_t> m_TextureData;
		uint32_t m_TextureStride;
		Utils::Rectangle m_DirtyRect;

		std::vector<uint8_t> m_PopupData;
		Utils::Rectangle m_PopupRect;
		bool m_PopupVisible;
		bool m_PopupValid;

		CefRefPtr<CefBrowser> m_Browser;
		CefRefPtr<CefMessageRouterBrowserSide> m_BrowserRouter;
		std::shared_ptr<Bridge::WebRendererQueryHandler> m_QueryHandler;

	public:
		explicit WebRendererHandler(LPDIRECT3DDEVICE9 p_Device, HWND p_Window);
		virtual ~WebRendererHandler() { }

		CefRefPtr<CefLifeSpanHandler> GetLifeSpanHandler() override { return this; }
		CefRefPtr<CefRenderHandler> GetRenderHandler() override { return this; }
		CefRefPtr<CefRequestHandler> GetRequestHandler() override { return this; }
		CefRefPtr<CefContextMenuHandler> GetContextMenuHandler() override { return this; }

		void OnAfterCreated(CefRefPtr<CefBrowser> p_Browser) override;
		void OnBeforeClose(CefRefPtr<CefBrowser> p_Browser) override;

		void OnPopupShow(CefRefPtr<CefBrowser> p_Browser, bool p_Show) override;
		void OnPopupSize(CefRefPtr<CefBrowser> p_Browser, const CefRect &p_Rect) override;

		bool GetViewRect(CefRefPtr<CefBrowser> p_Browser, CefRect &p_Rect) override;
		bool GetScreenInfo(CefRefPtr<CefBrowser> p_Browser, CefScreenInfo& p_ScreenInfo) override;
		bool GetScreenPoint(CefRefPtr<CefBrowser> p_Browser, int p_ViewX, int p_ViewY, int &p_ScreenX, int &p_ScreenY) override;
		void OnPaint(CefRefPtr<CefBrowser> p_Browser, PaintElementType p_Type, const RectList &p_DirtyRects, const void *p_Buffer, int p_Width, int p_Height) override;

		bool GetViewportInformation(uint32_t& p_Width, uint32_t& p_Height);
		bool Resize(uint32_t p_Width, uint32_t p_Height);

		void OnRenderProcessTerminated(CefRefPtr<CefBrowser> p_Browser, TerminationStatus p_Status) override;
		bool OnBeforeBrowse(CefRefPtr<CefBrowser> p_Browser, CefRefPtr<CefFrame> p_Frame, CefRefPtr<CefRequest> p_Request, bool p_IsRedirect) override;
		bool OnProcessMessageReceived(CefRefPtr<CefBrowser> p_Browser, CefProcessId p_SourceProcess, CefRefPtr<CefProcessMessage> p_Message) override;

		void OnBeforeContextMenu(CefRefPtr<CefBrowser> p_Browser, CefRefPtr<CefFrame> p_Frame, CefRefPtr<CefContextMenuParams> p_Params, CefRefPtr<CefMenuModel> p_Model) override;
		bool OnBeforePopup(CefRefPtr<CefBrowser> p_Browser, CefRefPtr<CefFrame> p_Frame, const CefString& p_Url, const CefString& p_FrameName, CefLifeSpanHandler::WindowOpenDisposition p_Disposition, bool p_UserGesture, const CefPopupFeatures& p_Features, CefWindowInfo& p_WindowInfo, CefRefPtr<CefClient>& p_Client, CefBrowserSettings& p_Settings, bool* p_NoJavaScript) override;

		uint8_t* GetTexture();
		uint32_t GetTextureLength();
		Utils::Rectangle GetTextureDirtyRect();
		void ResetTextureDirtyRect();

		CefRefPtr<CefBrowser> GetBrowser();
		void LockTexture();
		void UnlockTexture();

		std::shared_ptr<Bridge::WebRendererQueryHandler> GetQueryHandler() const { return m_QueryHandler; }

	protected:
		IMPLEMENT_REFCOUNTING(WebRendererHandler);
	};
}
