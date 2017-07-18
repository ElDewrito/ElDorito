#pragma once

namespace Forge
{
	namespace
	{
		namespace Geoemetry
		{
			struct VertexPosUV { float X, Y, Z, U, V; };

			const auto PT_TRIANGLESTRIP = 5;

			const VertexPosUV BOX_VERTICES[] =
			{
				// front
				{ -0.5f, 0.5f,-0.5f,  0.0f,  0.0f },
				{ 0.5f, 0.5f,-0.5f,  0.5f,  0.0f },
				{ -0.5f,-0.5f,-0.5f,  0.0f,  0.5f },
				{ 0.5f,-0.5f,-0.5f,  0.5f,  0.5f },
				// back
				{ -0.5f, 0.5f, 0.5f,  0.5f,  0.0f },
				{ -0.5f,-0.5f, 0.5f,  0.5f,  0.5f },
				{ 0.5f, 0.5f, 0.5f,  0.0f,  0.0f },
				{ 0.5f,-0.5f, 0.5f,  0.0f,  0.5f },
				// top
				{ -0.5f, 0.5f, 0.5f,  0.0f,  0.0f },
				{ 0.5f, 0.5f, 0.5f,  0.5f,  0.0f },
				{ -0.5f, 0.5f,-0.5f,  0.0f,  0.5f },
				{ 0.5f, 0.5f,-0.5f,  0.5f,  0.5f },
				// bottom
				{ -0.5f,-0.5f, 0.5f,  0.0f,  0.0f },
				{ -0.5f,-0.5f,-0.5f,  0.5f,  0.0f },
				{ 0.5f,-0.5f, 0.5f,  0.0f,  0.5f },
				{ 0.5f,-0.5f,-0.5f,  0.5f,  0.5f },
				// right
				{ 0.5f, 0.5f,-0.5f,  0.0f,  0.0f },
				{ 0.5f, 0.5f, 0.5f,  0.5f,  0.0f },
				{ 0.5f,-0.5f,-0.5f,  0.0f,  0.5f },
				{ 0.5f,-0.5f, 0.5f,  0.5f,  0.5f },
				// left
				{ -0.5f, 0.5f,-0.5f,  0.5f,  0.0f },
				{ -0.5f,-0.5f,-0.5f,  0.5f,  0.5f },
				{ -0.5f, 0.5f, 0.5f,  0.0f,  0.0f },
				{ -0.5f,-0.5f, 0.5f,  0.0f,  0.5f }
			};
		}
	}
}
