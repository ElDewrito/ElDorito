#pragma once
#include <d3d9.h>
#include <d3dx9.h>
#include <include/internal/cef_ptr.h>
#include <include/cef_app.h>
#include <memory>

namespace Anvil
{
	namespace Client
	{
		namespace Rendering
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

			class WebRenderer
			{
				CefRefPtr<WebRendererHandler> m_RenderHandler;
				CefRefPtr<CefApp> m_App;
				
				RendererState m_State;

				LPDIRECT3DTEXTURE9 m_Texture;
				LPDIRECT3DDEVICE9 m_Device;
				LPD3DXSPRITE m_Sprite;
				LPD3DXFONT m_Font;
				D3DXVECTOR3 m_Position;

			public:
				WebRenderer();
				static std::shared_ptr<WebRenderer> GetInstance();

				bool Init(const std::string &p_Url);

				bool InitRenderer(LPDIRECT3DDEVICE9 p_Device);

				bool Render(LPDIRECT3DDEVICE9 p_Device);

				bool Update();

				bool Initialized();
				bool IsRendering();

				bool ShowRenderer(bool p_Show, bool p_Overlay);

				bool Resize(uint32_t p_Width, uint32_t p_Height);

				bool UpdateMouse(uint32_t p_X, uint32_t p_Y);
				bool SendMouseEvent(uint32_t p_X, uint32_t p_Y, cef_mouse_button_type_t p_Button, bool p_Pressed);
				bool SendMouseWheelEvent(uint32_t p_X, uint32_t p_Y, int p_XDelta, int p_YDelta);
				bool SendKeyEvent(const CefKeyEvent &p_Event);

				bool OpenUrl(const std::string &p_Url);

				bool ExecuteJavascript(std::string p_Code);

				bool PreReset();
				bool PostReset();

				bool Shutdown();

				bool SetState(RendererState p_State);
				RendererState GetState();

				std::string GetUIDirectory();
			};
		}
	}
}
