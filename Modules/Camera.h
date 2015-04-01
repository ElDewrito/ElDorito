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

  std::string Info();

  void Tick(const std::chrono::duration<double>& Delta);
  bool Run(const std::vector<std::string>& Args);
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