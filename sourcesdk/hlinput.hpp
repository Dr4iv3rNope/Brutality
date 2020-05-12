#pragma once
#include "../valvesdk/interfaces.hpp"

#include "../build.hpp"
#include "vector.hpp"

#include "../util/pad.hpp"

namespace SourceSDK
{
	struct UserCmd;

	class Input
	{
	public:
		UserCmd* GetUserCmd(int sequence_number);

		#if BUILD_GAME_IS_GMOD
		UTIL_PAD(0, 140);
		bool cameraInterceptingMouse;
		bool cameraInThirdPerson;
		bool cameraMovingWithMouse;
		Vector3 cameraOffset;
		UTIL_PAD(1, 8);
		Angle cameraAngle;
		#endif
	};
}
