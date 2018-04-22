#include <functional>

namespace Patches::Camera
{
	void ApplyAll();
	void ApplyAfterTagsLoaded();

	// Callback for a Blam event handler.
	typedef std::function<void(int cameraMode)> CameraChangeCallback;

	// Registers a callback to be run when a Blam event fires.
	void OnCameraChange(CameraChangeCallback callback);

	void ApplyAll();
	void IncreaseLOD();

	void UpdateActiveFOV();
	void UpdateActiveFOVForCamera(int cameraFunc);
}