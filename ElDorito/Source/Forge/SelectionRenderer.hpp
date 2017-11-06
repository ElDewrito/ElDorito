#pragma once

namespace Forge::SelectionRenderer
{
	enum RendererImplementationType
	{
		eRendererSpecialHud = 0,
		eRendererImplicit = 1
	};


	void Initialize();
	void Update();
	void SetEnabled(bool enabled);
	void SetRendererType(RendererImplementationType type);
}

