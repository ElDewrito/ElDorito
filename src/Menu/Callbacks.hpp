#pragma once
#include "Settings.hpp"
#include <Awesomium/WebCore.h>

class Callbacks
{
public:
	static Settings* settings;

	void HUDShakeCallback(Awesomium::WebView* caller, const Awesomium::JSArray& args);
	void playerMarkerColorsCallback(Awesomium::WebView* caller, const Awesomium::JSArray& args);
	void fovCallback(Awesomium::WebView* caller, const Awesomium::JSArray& args);
	void hostStartTimerCallback(Awesomium::WebView* caller, const Awesomium::JSArray& args);
	void playerNameCallback(Awesomium::WebView* caller, const Awesomium::JSArray& args);
	void showFPSCallback(Awesomium::WebView* caller, const Awesomium::JSArray& args);
	void enableDXExtensionsCallback(Awesomium::WebView* caller, const Awesomium::JSArray& args);
	void keyboardInputCallback(Awesomium::WebView* caller, const Awesomium::JSArray& args);
	void toggleCrouchCallback(Awesomium::WebView* caller, const Awesomium::JSArray& args);
	void infantrySensCallback(Awesomium::WebView* caller, const Awesomium::JSArray& args);
	void vehicleSensCallback(Awesomium::WebView* caller, const Awesomium::JSArray& args);
	void mouseAccelerationCallback(Awesomium::WebView* caller, const Awesomium::JSArray& args);
	void mouseInvertedCallback(Awesomium::WebView* caller, const Awesomium::JSArray& args);
	void centeredCrosshairCallback(Awesomium::WebView* caller, const Awesomium::JSArray& args);
	void rawInputCallback(Awesomium::WebView* caller, const Awesomium::JSArray& args);
	void resolutionCallback(Awesomium::WebView* caller, const Awesomium::JSArray& args);
	void textureResolutionCallback(Awesomium::WebView* caller, const Awesomium::JSArray& args);
	void textureFilteringCallback(Awesomium::WebView* caller, const Awesomium::JSArray& args);
	void lightningQualityCallback(Awesomium::WebView* caller, const Awesomium::JSArray& args);
	void effectsQualityCallback(Awesomium::WebView* caller, const Awesomium::JSArray& args);
	void shadowQualityCallback(Awesomium::WebView* caller, const Awesomium::JSArray& args);
	void detailsLevelCallback(Awesomium::WebView* caller, const Awesomium::JSArray& args);
	void postProcessingCallback(Awesomium::WebView* caller, const Awesomium::JSArray& args);
	void motionBlurCallback(Awesomium::WebView* caller, const Awesomium::JSArray& args);
	void brightnessCallback(Awesomium::WebView* caller, const Awesomium::JSArray& args);
	void fullScreenCallback(Awesomium::WebView* caller, const Awesomium::JSArray& args);
	void VSyncCallback(Awesomium::WebView* caller, const Awesomium::JSArray& args);
	void antiAliasingCallback(Awesomium::WebView* caller, const Awesomium::JSArray& args);
	void masterVolumeCallback(Awesomium::WebView* caller, const Awesomium::JSArray& args);
	void sfxVolumeCallback(Awesomium::WebView* caller, const Awesomium::JSArray& args);
	void musicVolumeCallback(Awesomium::WebView* caller, const Awesomium::JSArray& args);
	void lanEnabledCallback(Awesomium::WebView* caller, const Awesomium::JSArray& args);
	void gameTypeCallback(Awesomium::WebView* caller, const Awesomium::JSArray& args);
	void mapCallback(Awesomium::WebView* caller, const Awesomium::JSArray& args);
	void forgeMapCallback(Awesomium::WebView* caller, const Awesomium::JSArray& args);
	void scoreToWinCallback(Awesomium::WebView* caller, const Awesomium::JSArray& args);
	void startGameCallback(Awesomium::WebView* caller, const Awesomium::JSArray& args);
	void connectCallback(Awesomium::WebView* caller, const Awesomium::JSArray& args);
};

