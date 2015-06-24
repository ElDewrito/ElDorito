#include "Callbacks.h"

// TODO: Does mouse filter even do anything? Should I add a callback for it?
// TODO: add callbacks for keyboard bindings
// TODO: call setters using WebView::executejavascript
// TODO: add GameOptions struct
// TODO: fix tint
// TODO: add transparency for armor customization (webView->SetTransparent(true) + it seems you need alpha blending)

Settings* Callbacks::settings = (Settings*) 0x23019B8;

void Callbacks::HUDShakeCallback(Awesomium::WebView* caller, const Awesomium::JSArray& args)
{
	// Applies ?
	settings->HUD_SHAKE = args.At(0).ToBoolean() ? 1 : 0;
}

void Callbacks::playerMarkerColorsCallback(Awesomium::WebView* caller, const Awesomium::JSArray& args)
{
	// Applies ?
	settings->PLAYER_MARKER_COLORS = args.At(0).ToInteger();
}

void Callbacks::fovCallback(Awesomium::WebView* caller, const Awesomium::JSArray& args)
{
	// Applies immediately
	settings->FOV_DOUBLED = args.At(0).ToInteger() * 2;
}

void Callbacks::hostStartTimerCallback(Awesomium::WebView* caller, const Awesomium::JSArray& args)
{

}

void Callbacks::playerNameCallback(Awesomium::WebView* caller, const Awesomium::JSArray& args)
{

}

void Callbacks::showFPSCallback(Awesomium::WebView* caller, const Awesomium::JSArray& args)
{

}

void Callbacks::enableDXExtensionsCallback(Awesomium::WebView* caller, const Awesomium::JSArray& args)
{

}

void Callbacks::keyboardInputCallback(Awesomium::WebView* caller, const Awesomium::JSArray& args)
{
	// Applies immediately
	settings->CONTROLS_METHOD = args.At(0).ToBoolean() ? 0 : 1;
}

void Callbacks::toggleCrouchCallback(Awesomium::WebView* caller, const Awesomium::JSArray& args)
{
	// Applies immediately
	settings->TOGGLE_CROUCH = args.At(0).ToBoolean() ? 1 : 0;
}

void Callbacks::infantrySensCallback(Awesomium::WebView* caller, const Awesomium::JSArray& args)
{
	// Applies immediately
	settings->INFANTRY_HORIZONTAL_MOUSE_SENS = args.At(0).ToInteger();
	settings->INFANTRY_VERTICAL_MOUSE_SENS = args.At(0).ToInteger();
}

void Callbacks::vehicleSensCallback(Awesomium::WebView* caller, const Awesomium::JSArray& args)
{
	// Applies immediately
	settings->VEHICLE_HORIZONTAL_MOUSE_SENS = args.At(0).ToInteger();
	settings->VEHICLE_VERTICAL_MOUSE_SENS = args.At(0).ToInteger();
}

void Callbacks::mouseAccelerationCallback(Awesomium::WebView* caller, const Awesomium::JSArray& args)
{
	// Applies immediately
	settings->MOUSE_ACCELERATION = args.At(0).ToInteger();
}

void Callbacks::mouseInvertedCallback(Awesomium::WebView* caller, const Awesomium::JSArray& args)
{
	// Applies immediately
	settings->INVERT_MOUSE = args.At(0).ToBoolean() ? 1 : 0;
}

void Callbacks::centeredCrosshairCallback(Awesomium::WebView* caller, const Awesomium::JSArray& args)
{

}

void Callbacks::rawInputCallback(Awesomium::WebView* caller, const Awesomium::JSArray& args)
{

}

void Callbacks::resolutionCallback(Awesomium::WebView* caller, const Awesomium::JSArray& args)
{
	// TODO: needs a hook to apply
	settings->HORIZONTAL_RESOLUTION = args.At(0).ToInteger();
	settings->VERTICAL_RESOLUTION = args.At(1).ToInteger();
}

void Callbacks::textureResolutionCallback(Awesomium::WebView* caller, const Awesomium::JSArray& args)
{
	// TODO: needs a game restart
	settings->TEXTURE_RESOLUTION = args.At(0).ToInteger();
}

void Callbacks::textureFilteringCallback(Awesomium::WebView* caller, const Awesomium::JSArray& args)
{
	// Applies ? I cannot tell a difference between low and high.
	settings->TEXTURE_FILTERING = args.At(0).ToInteger();
}

void Callbacks::lightningQualityCallback(Awesomium::WebView* caller, const Awesomium::JSArray& args)
{
	// Applies immediately
	settings->LIGHTNING_QUALITY = args.At(0).ToInteger();
}

void Callbacks::effectsQualityCallback(Awesomium::WebView* caller, const Awesomium::JSArray& args)
{
	// Applies immediately
	settings->EFFECTS_QUALITY = args.At(0).ToInteger();
}

void Callbacks::shadowQualityCallback(Awesomium::WebView* caller, const Awesomium::JSArray& args)
{
	// Applies immediately
	settings->SHADOW_QUALITY = args.At(0).ToInteger();
}

void Callbacks::detailsLevelCallback(Awesomium::WebView* caller, const Awesomium::JSArray& args)
{
	// Applies immediately
	settings->DETAILS_LEVEL = args.At(0).ToInteger();
}

void Callbacks::postProcessingCallback(Awesomium::WebView* caller, const Awesomium::JSArray& args)
{
	// Applies immediately
	settings->POSTPROCESSING = args.At(0).ToInteger();
}

void Callbacks::motionBlurCallback(Awesomium::WebView* caller, const Awesomium::JSArray& args)
{
	// Applies ?
	settings->MOTION_BLUR = args.At(0).ToBoolean() ? 1 : 0;
}

void Callbacks::brightnessCallback(Awesomium::WebView* caller, const Awesomium::JSArray& args)
{
	// Applies immediately
	settings->BRIGHTNESS = args.At(0).ToInteger();
}

void Callbacks::fullScreenCallback(Awesomium::WebView* caller, const Awesomium::JSArray& args)
{
	// TODO: needs a hook to apply
	settings->FULL_SCREEN = args.At(0).ToBoolean() ? 1 : 0;
}

void Callbacks::VSyncCallback(Awesomium::WebView* caller, const Awesomium::JSArray& args)
{
	// Applies ?
	settings->VSYNC = args.At(0).ToBoolean() ? 1 : 0;
}

void Callbacks::antiAliasingCallback(Awesomium::WebView* caller, const Awesomium::JSArray& args)
{
	// Applies ?
	settings->ANTI_ALIASING = args.At(0).ToBoolean() ? 1 : 0;
}

void Callbacks::masterVolumeCallback(Awesomium::WebView* caller, const Awesomium::JSArray& args)
{
	// TODO: not applied immediately
	settings->MASTER_VOLUME = args.At(0).ToInteger();
}

void Callbacks::sfxVolumeCallback(Awesomium::WebView* caller, const Awesomium::JSArray& args)
{
	// TODO: not applied immediately
	settings->SFX_VOLUME = args.At(0).ToInteger();
}

void Callbacks::musicVolumeCallback(Awesomium::WebView* caller, const Awesomium::JSArray& args)
{
	// TODO: not applied immediately
	settings->MUSIC_VOLUME = args.At(0).ToInteger();
}

void Callbacks::lanEnabledCallback(Awesomium::WebView* caller, const Awesomium::JSArray& args)
{

}

void Callbacks::gameTypeCallback(Awesomium::WebView* caller, const Awesomium::JSArray& args)
{

}

void Callbacks::mapCallback(Awesomium::WebView* caller, const Awesomium::JSArray& args)
{

}

void Callbacks::forgeMapCallback(Awesomium::WebView* caller, const Awesomium::JSArray& args)
{

}

void Callbacks::scoreToWinCallback(Awesomium::WebView* caller, const Awesomium::JSArray& args)
{

}

void Callbacks::startGameCallback(Awesomium::WebView* caller, const Awesomium::JSArray& args) {
	OutputDebugString("Start game!");
}

void Callbacks::serverCallback(Awesomium::WebView* caller, const Awesomium::JSArray& args)
{

}