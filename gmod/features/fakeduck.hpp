#pragma once
#include "../../build.hpp"

#if BUILD_GAME_IS_GMOD

namespace SourceSDK
{
	struct UserCmd;
}

namespace GarrysMod
{
	namespace Features
	{
		namespace FakeDuck
		{
			extern void Think(SourceSDK::UserCmd* cmd, bool& sendPacket) noexcept;
		}
	}
}

#endif
