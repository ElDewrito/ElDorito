#pragma once

namespace Forge
{
	class ObjectSet;
	namespace Selection
	{
		ObjectSet& GetSelection();

		void SelectEverything();
		void SelectAllMaterial();
		void SelectAll();
		void DeselectAllOf();

		void Expand(float amount);
		void Invert();

		void Clear();
		void Delete();
		bool Clone();

		void Update();
	}
}
