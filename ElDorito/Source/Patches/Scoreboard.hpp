#pragma once
#include <functional>

namespace Patches::Scoreboard
{
	void ApplyAll();

	typedef std::function<void()> ScoreUpdateCallback;
	void OnScoreUpdate(ScoreUpdateCallback callback);
}
