#include "Camera.h"

#include "../ElDorito.h"

// STL
#include <unordered_map>
#include <iostream>

static const std::unordered_map<std::string, CameraType> CameraTypeStrings = {
	{ "thirdperson", CameraType::Following },
	//{ "orbiting", CameraType::Orbiting },
	//{ "flying", CameraType::Flying },
	{ "firstperson", CameraType::FirstPerson },
	//{ "dead", CameraType::Dead },
	//{ "static", CameraType::Static },
	//{ "scripted", CameraType::Scripted },
	//{ "authored", CameraType::Authored }
};

static const std::unordered_map<CameraType, size_t> CameraTypeFunctions = {
	{ CameraType::FirstPerson, 0x166ACB0 },
	{CameraType::Following, 0x16724D4},
	{ CameraType::Dead, 0x16725DC }
};

Camera::Camera() 
	: m_debug1CameraPatch(0x325A80,
		{ 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 },
		{ 0xC7, 0x01, 0x30, 0x21, 0x67, 0x01 }),
		m_debug2CameraPatch(0x191525,
		{ 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 },
		{ 0xC7, 0x07, 0xE4, 0xA6, 0x65, 0x01 }),
		m_thirdPersonPatch(0x328640,
		{ 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 },
		{ 0xC7, 0x06, 0xD4, 0x24, 0x67, 0x01 }),
		m_firstPersonPatch(0x25F420,
		{ 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 },
		{ 0xC7, 0x06, 0xB0, 0xAC, 0x66, 0x01 }),
		m_deadPersonPatch(0x329E6F,
		{ 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 },
		{ 0xC7, 0x06, 0xDC, 0x25, 0x67, 0x01 })
{

}

Camera::~Camera()
{

}

std::string Camera::Info()
{
	std::string Info("Usage: camera (cameraMode)\nSupported Camera Modes:");
	for(auto &i : CameraTypeStrings)
	{
		Info += '\n';
		Info += 'Ã';
		Info += i.first;
	}
	Info += "\nPass \"reset\" to reset the camera mode.\n";

	return Info;
}

void Camera::Tick(const std::chrono::duration<double>& dt)
{

}

bool Camera::Run(const std::vector<std::string>& args)
{
	if(args.size())
	{
		if(args[1] == "reset")
		{
			setPatchesEnabled(false);
			setCameraType(CameraType::FirstPerson);
			std::cout << "Set camera reset." << std::endl;

			return true;
		}
		if(args.size() >= 2)
		{
			auto &type = CameraTypeStrings.find(args[1]);
			if(type != CameraTypeStrings.end())
			{
				setCameraType(type->second);
				setPatchesEnabled(true);

				std::cout << "Set camera mode to " << args[1] << " mode." << std::endl;
				return true;
			}
		}
	}
	return false; 
}

void Camera::setPatchesEnabled(bool areEnabled)
{
	if(areEnabled)
	{
		m_debug1CameraPatch.Apply();
		m_debug2CameraPatch.Apply();
		m_thirdPersonPatch.Apply();
		m_firstPersonPatch.Apply();
		m_deadPersonPatch.Apply();
	}
	else
	{
		m_debug1CameraPatch.Reset();
		m_debug2CameraPatch.Reset();
		m_thirdPersonPatch.Reset();
		m_firstPersonPatch.Reset();
		m_deadPersonPatch.Reset();
	}
}

void Camera::setCameraType(CameraType type)
{
	/*
	Patch cameraPatch(0x25F840,
	{ 0xB8, uint8_t(type), 0x00, 0x00, 0x00 },
	{ 0x90, 0x90, 0x90, 0x90, 0x90 }
	);
	cameraPatch.Apply();

	static const size_t firstPersonFnc = 0x166ACB0;
	*/

	auto &fncItr = CameraTypeFunctions.find(type);
	if(fncItr == CameraTypeFunctions.end())
		return;

	Pointer &directorPtr = ElDorito::GetMainTls(GameGlobals::Director::TLSOffset)[0];
	directorPtr(GameGlobals::Director::CameraFunctionIndex).Write(fncItr->second);
}

