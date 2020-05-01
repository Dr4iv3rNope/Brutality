#pragma once
#include "interfaces.hpp"

namespace SourceSDK
{
	enum class HostState
	{
		NewGame = 0,
		LoadGame,
		ChangeLevelSP,
		ChangeLevelMP,
		Run,
		GameShutdown,
		Shutdown,
		Restart,
	};

	SOURCE_SDK_INTERFACE_DECL(HostState, nextHostState);
}
