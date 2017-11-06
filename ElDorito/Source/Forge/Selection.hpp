#pragma once

namespace Forge
{
	class ObjectSet;
	namespace Selection
	{
		ObjectSet& GetSelection();

		void SelectAll();
		void DeselectAllOf();

		void Clear();
		void Delete();
		bool Clone();

		void Update();
	}
}
