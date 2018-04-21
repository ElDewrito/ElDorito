#pragma once
#include <d3d9.h>
#include <d3dx9.h>
#include <include/internal/cef_ptr.h>
#include <include/cef_app.h>
#include <memory>

#include "WebRendererApp.hpp"

namespace Anvil::Client::Rendering
{
	enum RendererState
	{
		RendererState_Disabled, // The renderer is disabled and not initialized
		RendererState_Shutdown,
		RendererState_Startup,
		RendererState_Hidden, // The renderer is initialized and just not rendering (ex, gameplay)
		RendererState_Overlay, // The renderer is showing an overlay, this mode is used for overlays ui replacements etc ingame
		RendererState_Shown, // The renderer is showing everything, and accepting input via mouse and keyboard.
		RendererState_Count
	};

	class WebRendererHandler;
	class WebRendererSchemeHandlerFactory;

	class WebRenderer
	{
		CefRefPtr<WebRendererHandler> m_RenderHandler;
		CefRefPtr<WebRendererApp> m_App;
				
		RendererState m_State;

		LPDIRECT3DTEXTURE9 m_Texture;
		LPDIRECT3DDEVICE9 m_Device;
		LPD3DXSPRITE m_Sprite;
		LPD3DXFONT m_Font;
		D3DXVECTOR3 m_Position;

	public:
		WebRenderer();
		static std::shared_ptr<WebRenderer> GetInstance();

		bool Init(const std::string &p_Url, bool p_EnableDebugging);

		bool InitRenderer(LPDIRECT3DDEVICE9 p_Device);

		bool Render(LPDIRECT3DDEVICE9 p_Device);

		bool Update();

		bool Initialized();
		bool IsRendering();

		bool ShowRenderer(bool p_Show, bool p_Overlay);

		bool Resize(uint32_t p_Width, uint32_t p_Height);

		bool SendMouseMoveEvent(const CefMouseEvent &p_Event, bool p_MouseLeave);
		bool SendMouseClickEvent(const CefMouseEvent &p_Event, CefBrowserHost::MouseButtonType p_Button, bool p_MouseUp, int p_ClickCount);
		bool SendMouseWheelEvent(const CefMouseEvent &p_Event, int p_DeltaX, int p_DeltaY);
		bool SendKeyEvent(const CefKeyEvent &p_Event);

		bool OpenUrl(const std::string &p_Url);
		bool Reload(bool p_IgnoreCache);

		bool RegisterScheme(const std::string &p_Name, const std::string &p_Directory);

		bool ExecuteJavascript(std::string p_Code);

		bool PreReset();
		bool PostReset();

		bool Shutdown();

		bool SetState(RendererState p_State);
		RendererState GetState();

		std::string GetUIDirectory();
	};
}
