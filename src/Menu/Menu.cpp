#include "Menu.hpp"
#include "..\Console\GameConsole.hpp"
#include <fstream>

Menu::Menu()
{
}

Menu::~Menu()
{
}

bool Menu::doesFileExist(const char *fileName)
{
	std::ifstream infile(fileName);
	return infile.good();
}

bool Menu::doesDirExist(const std::string& dirName_in)
{
	DWORD ftyp = GetFileAttributesA(dirName_in.c_str());
	if (ftyp == INVALID_FILE_ATTRIBUTES)
		return false;

	if (ftyp & FILE_ATTRIBUTE_DIRECTORY)
		return true;

	return false;
}

void Menu::bindCallbacks()
{
	/*Awesomium::JSValue result = webView->CreateGlobalJavascriptObject(Awesomium::WSLit("callbacks"));

	if (result.IsObject())
	{
		Awesomium::JSObject& callbacksObject = result.ToObject();
		methodDispatcher.Bind(callbacksObject, Awesomium::WSLit("HUDShake"), JSDelegate(&callbacks, &Callbacks::HUDShakeCallback));
		methodDispatcher.Bind(callbacksObject, Awesomium::WSLit("playerMarkerColors"), JSDelegate(&callbacks, &Callbacks::playerMarkerColorsCallback));
		methodDispatcher.Bind(callbacksObject, Awesomium::WSLit("fov"), JSDelegate(&callbacks, &Callbacks::fovCallback));
		methodDispatcher.Bind(callbacksObject, Awesomium::WSLit("hostStartTimer"), JSDelegate(&callbacks, &Callbacks::hostStartTimerCallback));
		methodDispatcher.Bind(callbacksObject, Awesomium::WSLit("playerName"), JSDelegate(&callbacks, &Callbacks::playerNameCallback));
		methodDispatcher.Bind(callbacksObject, Awesomium::WSLit("showFPS"), JSDelegate(&callbacks, &Callbacks::showFPSCallback));
		methodDispatcher.Bind(callbacksObject, Awesomium::WSLit("enableDXExtensions"), JSDelegate(&callbacks, &Callbacks::enableDXExtensionsCallback));
		methodDispatcher.Bind(callbacksObject, Awesomium::WSLit("keyboardInput"), JSDelegate(&callbacks, &Callbacks::keyboardInputCallback));
		methodDispatcher.Bind(callbacksObject, Awesomium::WSLit("toggleCrouch"), JSDelegate(&callbacks, &Callbacks::toggleCrouchCallback));
		methodDispatcher.Bind(callbacksObject, Awesomium::WSLit("infantrySens"), JSDelegate(&callbacks, &Callbacks::infantrySensCallback));
		methodDispatcher.Bind(callbacksObject, Awesomium::WSLit("vehicleSens"), JSDelegate(&callbacks, &Callbacks::vehicleSensCallback));
		methodDispatcher.Bind(callbacksObject, Awesomium::WSLit("mouseAcceleration"), JSDelegate(&callbacks, &Callbacks::mouseAccelerationCallback));
		methodDispatcher.Bind(callbacksObject, Awesomium::WSLit("mouseInverted"), JSDelegate(&callbacks, &Callbacks::mouseInvertedCallback));
		methodDispatcher.Bind(callbacksObject, Awesomium::WSLit("centeredCrosshair"), JSDelegate(&callbacks, &Callbacks::centeredCrosshairCallback));
		methodDispatcher.Bind(callbacksObject, Awesomium::WSLit("rawInput"), JSDelegate(&callbacks, &Callbacks::rawInputCallback));
		methodDispatcher.Bind(callbacksObject, Awesomium::WSLit("resolution"), JSDelegate(&callbacks, &Callbacks::resolutionCallback));
		methodDispatcher.Bind(callbacksObject, Awesomium::WSLit("textureResolution"), JSDelegate(&callbacks, &Callbacks::textureResolutionCallback));
		methodDispatcher.Bind(callbacksObject, Awesomium::WSLit("textureFiltering"), JSDelegate(&callbacks, &Callbacks::textureFilteringCallback));
		methodDispatcher.Bind(callbacksObject, Awesomium::WSLit("lightningQuality"), JSDelegate(&callbacks, &Callbacks::lightningQualityCallback));
		methodDispatcher.Bind(callbacksObject, Awesomium::WSLit("effectsQuality"), JSDelegate(&callbacks, &Callbacks::effectsQualityCallback));
		methodDispatcher.Bind(callbacksObject, Awesomium::WSLit("shadowQuality"), JSDelegate(&callbacks, &Callbacks::shadowQualityCallback));
		methodDispatcher.Bind(callbacksObject, Awesomium::WSLit("detailsLevel"), JSDelegate(&callbacks, &Callbacks::detailsLevelCallback));
		methodDispatcher.Bind(callbacksObject, Awesomium::WSLit("postProcessing"), JSDelegate(&callbacks, &Callbacks::postProcessingCallback));
		methodDispatcher.Bind(callbacksObject, Awesomium::WSLit("motionBlur"), JSDelegate(&callbacks, &Callbacks::motionBlurCallback));
		methodDispatcher.Bind(callbacksObject, Awesomium::WSLit("brightness"), JSDelegate(&callbacks, &Callbacks::brightnessCallback));
		methodDispatcher.Bind(callbacksObject, Awesomium::WSLit("fullScreen"), JSDelegate(&callbacks, &Callbacks::fullScreenCallback));
		methodDispatcher.Bind(callbacksObject, Awesomium::WSLit("VSync"), JSDelegate(&callbacks, &Callbacks::VSyncCallback));
		methodDispatcher.Bind(callbacksObject, Awesomium::WSLit("antiAliasing"), JSDelegate(&callbacks, &Callbacks::antiAliasingCallback));
		methodDispatcher.Bind(callbacksObject, Awesomium::WSLit("masterVolume"), JSDelegate(&callbacks, &Callbacks::masterVolumeCallback));
		methodDispatcher.Bind(callbacksObject, Awesomium::WSLit("sfxVolume"), JSDelegate(&callbacks, &Callbacks::sfxVolumeCallback));
		methodDispatcher.Bind(callbacksObject, Awesomium::WSLit("musicVolume"), JSDelegate(&callbacks, &Callbacks::musicVolumeCallback));
		methodDispatcher.Bind(callbacksObject, Awesomium::WSLit("networkType"), JSDelegate(&callbacks, &Callbacks::networkTypeCallback));
		methodDispatcher.Bind(callbacksObject, Awesomium::WSLit("gameType"), JSDelegate(&callbacks, &Callbacks::gameTypeCallback));
		methodDispatcher.Bind(callbacksObject, Awesomium::WSLit("map"), JSDelegate(&callbacks, &Callbacks::mapCallback));
		methodDispatcher.Bind(callbacksObject, Awesomium::WSLit("forgeMap"), JSDelegate(&callbacks, &Callbacks::forgeMapCallback));
		methodDispatcher.Bind(callbacksObject, Awesomium::WSLit("scoreToWin"), JSDelegate(&callbacks, &Callbacks::scoreToWinCallback));
		methodDispatcher.Bind(callbacksObject, Awesomium::WSLit("startGame"), JSDelegate(&callbacks, &Callbacks::startGameCallback));
		methodDispatcher.Bind(callbacksObject, Awesomium::WSLit("connect"), JSDelegate(&callbacks, &Callbacks::connectCallback));
	}

	webView->set_js_method_handler(&methodDispatcher);*/
}

void Menu::toggleMenu()
{
	STARTUPINFO si;
	PROCESS_INFORMATION pi;

	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));

	CreateProcess("custom_menu.exe", 0, 0, 0, false, 0, 0, 0, &si, &pi);

	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);
}
