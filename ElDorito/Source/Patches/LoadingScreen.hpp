#pragma once
#include <string>
#include <cstdint>
#include <memory>

namespace Patches::LoadingScreen
{
	void ApplyAll();

	// Interface for an object that implements the UI for a loading screen.
	class LoadingScreenUi
	{
	public:
		virtual ~LoadingScreenUi() { }

		// Called when the loading screen should be shown.
		virtual void Show(const std::string &mapPath) = 0;

		// Called when loading begins.
		virtual void Begin(uint32_t totalBytes) = 0;

		// Called when more bytes have been processed.
		virtual void UpdateProgress(uint32_t bytes) = 0;

		// Called when the loading screen should be hidden.
		virtual void Hide() = 0;
	};

	// Sets the active loading screen UI object.
	void SetUi(std::shared_ptr<LoadingScreenUi> ui);
}