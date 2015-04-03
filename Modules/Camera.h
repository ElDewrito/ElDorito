#pragma once

#include "ElModule.h"
#include <inttypes.h>
#include "../BlamTypes.h"

#include "../Patch.h"

class Camera : public ElModule
{
public:
	Camera();
	~Camera();

	void Tick(const std::chrono::duration<double>& Delta);

	// Return true on success
	bool Run(const std::vector<std::string>& Arguments);

	// Command and usage info
	std::string Info() const;

	// Suggest auto-complete strings for arugments
	std::string Suggest(const std::vector<std::string>& Arguments) const;

private:
	void setPatchesEnabled(bool areEnabled);
	void setCameraType(CameraType type);

	// Transition functions
	Patch m_debug1CameraPatch;
	Patch m_debug2CameraPatch;
	Patch m_thirdPersonPatch;
	Patch m_firstPersonPatch;
	Patch m_deadPersonPatch;
};