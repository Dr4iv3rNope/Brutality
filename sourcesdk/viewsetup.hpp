#pragma once
#include "sdk.hpp"
#include "vector.hpp"

#include "../util/pad.hpp"

namespace SourceSDK
{
	struct ViewSetup final
	{
		UTIL_PAD(0, 0x20);

		struct
		{
			bool enabled;

			float left, top, right, bottom;
		} ortho;

		float fov;
		float fovViewmodel;

		Vector3 origin;
		Angle angles;

		float zNear;
		float zFar;
		float zNearViewmodel;
		float zFarViewmodel;

		bool renderToSubrectOfLargerScreen;
		float aspectRatio;

		struct
		{
			bool enabled;

			float top, bottom, left, right;
		} offCenter;

		bool bloomAndToneMapping;
		bool cacheFullSceneState;
	};
}
