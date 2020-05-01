#pragma once

namespace SourceSDK
{
	enum class FCvar : int
	{
		None,
		Unregistered = 1 << 0,
		DevelopmentOnly = 1 << 1,
		GameDLL = 1 << 2,
		ClientDLL = 1 << 3,
		Hidden = 1 << 4,

		// convar only

		Protected = 1 << 5,
		SinglePlayerOnly = 1 << 6,
		Archive = 1 << 7,
		Notify = 1 << 8,
		UserInfo = 1 << 9,
		PrintableOnly = 1 << 10,
		Unlogged = 1 << 11,
		NeverAsString = 1 << 12,
		Replicated = 1 << 13,
		Cheat = 1 << 14,
		Demo = 1 << 16,
		DontRecord = 1 << 17,
		NotConnected = 1 << 22,
		ArchieveXBox = 1 << 24,
		ServerCanExecute = 1 << 28,
		ServerCannotQuery = 1 << 29,
		ClientCmdCanExecute = 1 << 30
	};
}
