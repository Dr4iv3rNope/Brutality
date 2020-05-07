#include "worldtoscreen.hpp"
#include "matrix.hpp"

#include "../hooks/renderview.hpp"

#include "../imgui/imgui.h"

SourceSDK::W2SOutput SourceSDK::WorldToScreen(const Vector3& world)
{
	static VMatrix worldToScreen;
	Hooks::CopyLastWorldToScreenMatrix(worldToScreen);

	float w =
		worldToScreen[3][0] * world.X() +
		worldToScreen[3][1] * world.Y() +
		worldToScreen[3][2] * world.Z() + worldToScreen[3][3];
	
	if (w > 0.001f)
	{
		static auto imguiIO { ImGui::GetIO() };
		auto display_size = imguiIO.DisplaySize;

		float clip_x =
			worldToScreen[0][0] * world.X() +
			worldToScreen[0][1] * world.Y() +
			worldToScreen[0][2] * world.Z() +
			worldToScreen[0][3];

		float clip_y =
			worldToScreen[1][0] * world.X() +
			worldToScreen[1][1] * world.Y() +
			worldToScreen[1][2] * world.Z() +
			worldToScreen[1][3];

		return W2SOutput
		{
			Vector2(
				(display_size.x / 2) * (1 + (clip_x / w)),
				(display_size.y / 2) * (1 - (clip_y / w))
			)
		};
	}
	else
		return W2SOutput {};
}
