#pragma once

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
}
