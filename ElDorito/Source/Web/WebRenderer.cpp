#include "WebRenderer.hpp"
#include "WebRendererHandler.hpp"
#include "WebRendererApp.hpp"
#include "WebRendererSchemeHandler.hpp"
#include "WebRendererSchemeHandlerFactory.hpp"
#include <cef_app.h>
#include <cef_origin_whitelist.h>

#include <d3dx9.h>
#include "Logger.hpp"
#include "Bridge/WebRendererQueryHandler.hpp"

// 1 to enable multi_threaded_message_loop
#define MULTITHREADED 1

using Anvil::Client::Rendering::WebRenderer;

WebRenderer::WebRenderer() :
	m_RenderHandler(nullptr),
	m_App(nullptr),
	m_State(RendererState_Disabled),
	m_Texture(nullptr),
	m_Device(nullptr),
	m_Sprite(nullptr),
	m_Font(nullptr),
	m_Position(D3DXVECTOR3(0, 0, 0))
{
	WriteLog("WebRenderer Ctor.");
}

std::shared_ptr<WebRenderer> WebRenderer::GetInstance()
{
	static auto s_Instance = std::make_shared<WebRenderer>();
	return s_Instance;
}

std::string WebRenderer::GetUIDirectory()
{
	char s_PathBuffer[MAX_PATH] = { 0 };
	auto s_BufferSize = sizeof(s_PathBuffer);

	auto s_Result = GetModuleFileName(nullptr, s_PathBuffer, s_BufferSize);
	if (!s_Result)
	{
		WriteLog("Could not get current executable file path (%x).", GetLastError());
	}

	auto s_LocalExecutable = std::string(s_PathBuffer);
	auto s_RunningDirectory = s_LocalExecutable.substr(0, s_LocalExecutable.find_last_of("\\/"));

	return s_RunningDirectory + "/mods/ui/web";
}

bool WebRenderer::Init(const std::string &p_Url, bool p_EnableDebugging)
{
	// Do not try to initialize unless we are already at a disabled state
	if (GetState() != RendererState_Startup)
		return false;

	if (!m_Device)
		return false;

	m_App = new WebRendererApp();

	if (!m_App)
	{
		WriteLog("App (%p) is invalid.", m_App);
		Shutdown();
		return false;
	}

	CefMainArgs s_Args(GetModuleHandle(nullptr));

	auto s_Result = CefExecuteProcess(s_Args, m_App, nullptr);
	if (s_Result >= 0)
	{
		WriteLog("CefExecuteProcess failed (%x).", s_Result);
		TerminateProcess(GetCurrentProcess(), 0); // TODO: Take less drastic measures...
		return false;
	}

	CefSettings s_Settings;
#if MULTITHREADED
	s_Settings.multi_threaded_message_loop = true;
#else
	s_Settings.multi_threaded_message_loop = false;
#endif
	CefString(&s_Settings.product_version) = "ElDewrito";
	CefString(&s_Settings.browser_subprocess_path) = "custom_menu.exe";
	CefString(&s_Settings.log_file) = "custom_menu.log";
	s_Settings.no_sandbox = true;
	s_Settings.pack_loading_disabled = false;
	s_Settings.windowless_rendering_enabled = true;
	s_Settings.ignore_certificate_errors = true;
	s_Settings.single_process = false;
	s_Settings.persist_user_preferences = true;
	CefString(&s_Settings.resources_dir_path) = boost::filesystem::canonical("mods/ui/resources").c_str(); // resources-dir-patch requires an absolute path
	CefString(&s_Settings.locales_dir_path) = "mods/ui/resources/locales";
	CefString(&s_Settings.user_data_path) = "mods/ui/cache";
	CefString(&s_Settings.cache_path) = "mods/ui/cache";
	if (p_EnableDebugging)
	{
		s_Settings.log_severity = LOGSEVERITY_VERBOSE;
		s_Settings.remote_debugging_port = 8884;
		WriteLog("Web renderer debugging enabled on port %d", s_Settings.remote_debugging_port);
	}
	else
	{
		s_Settings.log_severity = LOGSEVERITY_WARNING;
	}

	// Delete the log file so that it doesn't grow huge and take up space
	DeleteFileW(s_Settings.log_file.str);

	if (!CefInitialize(s_Args, s_Settings, m_App, nullptr))
	{
		WriteLog("CefInitialize failed.");
		ExitProcess(0); // TODO: Take less drastic measures
		return false;
	}

	auto s_UIDirectory = GetUIDirectory();

	WriteLog("UI Directory: %s", s_UIDirectory.c_str());

	CefWindowInfo s_WindowInfo;
	CefBrowserSettings s_BrowserSettings;

	s_BrowserSettings.windowless_frame_rate = 60;
	s_BrowserSettings.webgl = STATE_ENABLED;
	s_BrowserSettings.javascript = STATE_ENABLED;
	s_BrowserSettings.local_storage = STATE_ENABLED;
	s_BrowserSettings.databases = STATE_ENABLED;
	s_BrowserSettings.application_cache = STATE_ENABLED;
	s_BrowserSettings.file_access_from_file_urls = STATE_DISABLED;
	s_BrowserSettings.javascript_close_windows = STATE_DISABLED;
	s_BrowserSettings.javascript_open_windows = STATE_DISABLED;
	s_BrowserSettings.javascript_access_clipboard = STATE_DISABLED;
	s_BrowserSettings.universal_access_from_file_urls = STATE_DISABLED;
	s_BrowserSettings.remote_fonts = STATE_ENABLED;
	s_BrowserSettings.image_loading = STATE_ENABLED;

	D3DDEVICE_CREATION_PARAMETERS s_Parameters;
	ZeroMemory(&s_Parameters, sizeof(s_Parameters));

	s_Result = m_Device->GetCreationParameters(&s_Parameters);
	if (FAILED(s_Result))
	{
		WriteLog("Could not get the creation parameters.");
		Shutdown();
		return false;
	}

	//std::string s_ContainerPath = "http://scooterpsu.github.io/";
	std::string s_ContainerPath = p_Url;

	WriteLog("Container Path: %s.", s_ContainerPath.c_str());

	s_WindowInfo.SetAsWindowless(s_Parameters.hFocusWindow, true);

	m_RenderHandler = new WebRendererHandler(m_Device, s_Parameters.hFocusWindow);
	if (!m_RenderHandler)
	{
		WriteLog("WebRendererHandler failed to initialize.");
		Shutdown();
		return false;
	}

	// Register our custom handlers
	RegisterScheme("dew", GetUIDirectory());
	CefAddCrossOriginWhitelistEntry("dew://ui", "http", "", true);

	// Register a medals:// scheme to point to mods\medals (and ensure the directory for a default pack exists)
	boost::system::error_code error;
	boost::filesystem::create_directories("mods\\medals\\default", error);
	RegisterScheme("medals", "mods\\medals");

	auto s_RequestContext = CefRequestContext::GetGlobalContext();
	if (!CefBrowserHost::CreateBrowser(s_WindowInfo, m_RenderHandler.get(), s_ContainerPath.c_str(), s_BrowserSettings, s_RequestContext))
	{
		WriteLog("Failed to initialize WebRenderer.");
		Shutdown();
		return false;
	}

	uint32_t s_Width = 0, s_Height = 0;
	if (!m_RenderHandler->GetViewportInformation(s_Width, s_Height))
	{
		Shutdown();
		return false;
	}

	if (!Resize(s_Width, s_Height))
	{
		WriteLog("Resize failed.");
		Shutdown();
		return false;
	}

	SetState(RendererState_Hidden);

	return true;
}

bool WebRenderer::InitRenderer(LPDIRECT3DDEVICE9 p_Device)
{
	// In order to enter the state, we have to have already hit the startup state, which init leaves hanging for the dx thread to catch up and call this
	// ^ Thats backwards, initrenderer should be called first
	if (GetState() != RendererState_Disabled)
		return false;

	if (!p_Device)
	{
		WriteLog("Device is invalid.");
		Shutdown();
		return false;
	}

	m_Device = p_Device;

	// Create a sprite for rendering
	auto s_Result = D3DXCreateSprite(m_Device, &m_Sprite);
	if (FAILED(s_Result))
	{
		WriteLog("Could not create sprite (%x).", s_Result);
		Shutdown();
		return false;
	}

	// Create a font for debug purposes
	s_Result = D3DXCreateFont(m_Device, 16, 0, FW_NORMAL, 1, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "Arial", &m_Font);
	if (FAILED(s_Result))
	{
		WriteLog("Could not create font (%x).", s_Result);
		Shutdown();
		return false;
	}

	D3DVIEWPORT9 s_Viewport;
	s_Result = p_Device->GetViewport(&s_Viewport);
	if (FAILED(s_Result))
	{
		WriteLog("Could not get viewport (%x).", s_Result);
		Shutdown();
		return false;
	}

	WriteLog("WebRenderer InitRenderer.");

	// We leave the renderer state in startup so the init function would work
	SetState(RendererState_Startup);

	return true;
}

bool WebRenderer::Render(LPDIRECT3DDEVICE9 p_Device)
{
	auto s_State = GetState();
	if (s_State != RendererState_Overlay && s_State != RendererState_Shown)
		return false;

	if (!p_Device)
	{
		WriteLog("Device is invalid.");
		return false;
	}

	if (!m_Sprite || !m_Font || !m_Texture || !m_RenderHandler)
	{
		WriteLog("Sprite, Font, Texture, Renderhandler invalid.");
		return false;
	}

	m_RenderHandler->LockTexture();

	if (!m_RenderHandler->GetTextureDirtyRect().IsEmpty())
	{
		D3DLOCKED_RECT s_Rect;
		auto s_Result = m_Texture->LockRect(0, &s_Rect, nullptr, D3DLOCK_DISCARD);
		if (SUCCEEDED(s_Result))
		{
			uint32_t s_Width = 0, s_Height = 0;
			if (!m_RenderHandler->GetViewportInformation(s_Width, s_Height))
			{
				m_Texture->UnlockRect(0);
				m_RenderHandler->UnlockTexture();
				return false;
			}

			auto s_TextureData = m_RenderHandler->GetTexture();
			if (!s_TextureData)
			{
				m_Texture->UnlockRect(0);
				m_RenderHandler->UnlockTexture();
				return false;
			}

			// We cannot assume that s_Rect.Pitch == s_Width * 4, so a rectangular copy needs to be done
			// Otherwise there is corruption at certain resolutions (e.g. 1680x1050)
			Utils::Rectangle s_CopyRect(0, 0, s_Width, s_Height);
			Utils::Rectangle::Copy(s_Rect.pBits, 0, 0, s_Rect.Pitch, s_TextureData, s_CopyRect, s_Width * 4, 4);

			m_Texture->UnlockRect(0);
			m_RenderHandler->ResetTextureDirtyRect();
		}
	}

	m_RenderHandler->UnlockTexture();

	if (GetState() == RendererState_Shown)
		p_Device->Clear(1, nullptr, D3DCLEAR_TARGET, D3DCOLOR_ARGB(255, 128, 128, 128), 0, 0);

	m_Sprite->Begin(D3DXSPRITE_ALPHABLEND);

	m_Sprite->Draw(m_Texture, nullptr, nullptr, &m_Position, 0xFFFFFFFF);

	m_Sprite->Flush();

	m_Sprite->End();

	return true;
}

bool WebRenderer::Update()
{
	if (!Initialized())
		return false;
#if !MULTITHREADED
	CefDoMessageLoopWork();
#endif

	m_RenderHandler->GetQueryHandler()->Update();
	return true;
}

bool WebRenderer::Initialized()
{
	auto s_State = GetState();
	return (s_State == RendererState_Overlay
		|| s_State == RendererState_Shown
		|| s_State == RendererState_Hidden);
}

bool WebRenderer::IsRendering()
{
	return GetState() == RendererState_Overlay || GetState() == RendererState_Shown;
}

bool WebRenderer::ShowRenderer(bool p_Show, bool p_Overlay)
{
	// NOTE: Do NOT call WasHidden() because it causes issues where some UI controls (e.g. scrollbars) do not update properly

	auto s_State = GetState();
	if (s_State == RendererState_Hidden && p_Show)
	{
		SetState(p_Overlay ? RendererState_Overlay : RendererState_Shown);
		if (m_RenderHandler)
		{
			auto s_Browser = m_RenderHandler->GetBrowser();
			if (s_Browser)
				s_Browser->GetHost()->SendFocusEvent(true);
		}
		return true;
	}

	if ((s_State == RendererState_Overlay || s_State == RendererState_Shown) && !p_Show)
	{
		SetState(RendererState_Hidden);
		if (m_RenderHandler)
		{
			auto s_Browser = m_RenderHandler->GetBrowser();
			if (s_Browser)
				s_Browser->GetHost()->SendFocusEvent(false);
		}
		return true;
	}

	return false;
}

bool WebRenderer::Resize(uint32_t p_Width, uint32_t p_Height)
{
	if (!m_RenderHandler)
		return false;

	// Resize the render handler

	auto s_Result = m_RenderHandler->Resize(p_Width, p_Height);
	if (!s_Result)
	{
		WriteLog("Resize failed.");
		return false;
	}

	// Re-create the render texture

	if (m_Texture)
	{
		m_Texture->Release();
		m_Texture = nullptr;
	}
	if (FAILED(m_Device->CreateTexture(p_Width, p_Height, 1, D3DUSAGE_DYNAMIC, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &m_Texture, nullptr)))
		return false;

	// Notify the browser that it was resized

	auto s_Browser = m_RenderHandler->GetBrowser().get();
	if (s_Browser)
	{
		s_Browser->GetHost()->NotifyScreenInfoChanged();
		s_Browser->GetHost()->WasResized();
		s_Browser->GetHost()->Invalidate(PET_VIEW);
		s_Browser->GetHost()->Invalidate(PET_POPUP);
	}
	return true;
}

bool WebRenderer::SendMouseMoveEvent(const CefMouseEvent &p_Event, bool p_MouseLeave)
{
	if (!m_RenderHandler || !IsRendering())
		return false;

	auto s_Browser = m_RenderHandler->GetBrowser().get();
	if (!s_Browser)
		return false;

	s_Browser->GetHost()->SendMouseMoveEvent(p_Event, p_MouseLeave);
	return true;
}

bool WebRenderer::SendMouseClickEvent(const CefMouseEvent &p_Event, CefBrowserHost::MouseButtonType p_Button, bool p_MouseUp, int p_ClickCount)
{
	if (!m_RenderHandler || !IsRendering())
		return false;

	auto s_Browser = m_RenderHandler->GetBrowser().get();
	if (!s_Browser)
		return false;

	s_Browser->GetHost()->SendMouseClickEvent(p_Event, p_Button, p_MouseUp, p_ClickCount);
	return true;
}

bool WebRenderer::SendMouseWheelEvent(const CefMouseEvent &p_Event, int p_DeltaX, int p_DeltaY)
{
	if (!m_RenderHandler || !IsRendering())
		return false;

	auto s_Browser = m_RenderHandler->GetBrowser().get();
	if (!s_Browser)
		return false;

	s_Browser->GetHost()->SendMouseWheelEvent(p_Event, p_DeltaX, p_DeltaY);
	return true;
}

bool WebRenderer::SendKeyEvent(const CefKeyEvent &p_Event)
{
	if (!m_RenderHandler || !IsRendering())
		return false;

	auto s_Browser = m_RenderHandler->GetBrowser().get();
	if (!s_Browser)
		return false;

	s_Browser->GetHost()->SendKeyEvent(p_Event);
	return true;
}

bool WebRenderer::OpenUrl(const std::string &p_Url)
{
	if (!m_RenderHandler || !Initialized())
		return false;

	auto s_Browser = m_RenderHandler->GetBrowser().get();
	if (!s_Browser)
		return false;

	s_Browser->GetFocusedFrame()->LoadURL(p_Url);
	return true;
}

bool WebRenderer::Reload(bool p_IgnoreCache)
{
	if (!m_RenderHandler || !Initialized())
		return false;

	auto s_Browser = m_RenderHandler->GetBrowser().get();
	if (!s_Browser)
		return false;

	if (p_IgnoreCache)
	{
		WebRendererSchemeHandler::ClearCache();
		s_Browser->ReloadIgnoreCache();
	}
	else
	{
		s_Browser->Reload();
	}
	return true;
}

bool WebRenderer::RegisterScheme(const std::string &p_Name, const std::string &p_Directory)
{
	if (!boost::filesystem::exists(p_Directory))
		return false;
	auto s_SchemeHandlerFactory = new WebRendererSchemeHandlerFactory(p_Name, p_Directory);
	if (!CefRegisterSchemeHandlerFactory(p_Name, "", s_SchemeHandlerFactory))
		return false;

	// Allow additional protocols to be accessed from dew://
	if (p_Name != "dew")
		CefAddCrossOriginWhitelistEntry("dew://*", p_Name, "", true);

	return true;
}

bool WebRenderer::ExecuteJavascript(std::string p_Code)
{
	auto s_Browser = m_RenderHandler->GetBrowser().get();
	if (!s_Browser)
		return false;

	s_Browser->GetMainFrame()->ExecuteJavaScript(p_Code, "internal", 0);

	return true;
}

bool WebRenderer::PreReset()
{
	if (!m_Device)
		return false;

	if (m_Sprite)
		m_Sprite->OnLostDevice();

	if (m_Font)
		m_Font->OnLostDevice();

	return true;
}

bool WebRenderer::PostReset()
{
	if (!m_Device)
		return false;

	if (m_Sprite)
		m_Sprite->OnResetDevice();

	if (m_Font)
		m_Font->OnResetDevice();

	return true;
}

bool WebRenderer::Shutdown()
{
	// If we are already disabled, don't try to free the elements, and if we are already shutting down don't call it again
	if (m_State == RendererState_Disabled || m_State == RendererState_Shutdown)
		return false;

	// Set us to be in the "shutdown" state and free resources
	SetState(RendererState_Shutdown);

	// Release handles
	m_App = nullptr;

	if (m_RenderHandler)
	{
		// Ask the browser to close
		auto s_Browser = m_RenderHandler->GetBrowser();
		if (s_Browser)
			s_Browser->GetHost()->CloseBrowser(true);

		// Release the handler
		m_RenderHandler = nullptr;

		// MAJOR HAX: Wait for s_Browser to be the only browser reference left.
		// CEF is dumb and will throw a fit and refuse to shutdown otherwise.
		// What you're supposed to do is wait for CefLifeSpanHandler::OnBeforeClose()
		// and then signal your main thread that it's able to close, but that can
		// still lead to race conditions in multi-threaded mode if your main thread
		// tries to shut down before the browser is actually released by CEF.
		while (!s_Browser->HasOneRef())
			Sleep(0);
		s_Browser = nullptr;
	}

	CefClearSchemeHandlerFactories();
	CefShutdown();

	try
	{
		// Set us back to our disabled state
		SetState(RendererState_Disabled);
	}
	catch (...)
	{
		return false;
	}

	return true;
}

Anvil::Client::Rendering::RendererState WebRenderer::GetState()
{
	return m_State;
}

bool WebRenderer::SetState(Anvil::Client::Rendering::RendererState p_State)
{
	// If the state is already set, go away
	if (m_State == p_State)
		return false;

	// Bounds checking
	if (m_State >= RendererState_Count || m_State < RendererState_Disabled)
		return false;

	m_State = p_State;

	return true;
}