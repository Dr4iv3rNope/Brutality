#pragma once
#include "sdk.hpp"
#include "interfaces.hpp"
#include "vector.hpp"
#include "buttoncode.hpp"

namespace SourceSDK
{
	struct UserCmd;
	class Weapon;

	struct CameraThirdData
	{
		float pitch;
		float yaw;
		float dist;
		float lag;
		Vector vecHullMin;
		Vector vecHullMax;
	};

	struct KButton
	{
		int down[2];
		int state;
	};

	class Input
	{
	public:
		UserCmd* GetUserCmd(int sequence_number);
	};

	SOURCE_SDK_INTERFACE_DECL(Input, input);
}
