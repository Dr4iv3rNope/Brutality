#pragma once
#include "../util/flags.hpp"

namespace SourceSDK
{
	UTIL_FLAGS(ConCmdFlags)
	{
		ConCmdFlags_None = 0,

		ConCmdFlags_Unregistered = 1 << 0,
		ConCmdFlags_DevelopmentOnly = 1 << 1,
		ConCmdFlags_GameDLL = 1 << 2,
		ConCmdFlags_ClientDLL = 1 << 3,
		ConCmdFlags_Hidden = 1 << 4,

		// convar only

		ConCmdFlags_Protected = 1 << 5,
		ConCmdFlags_SinglePlayerOnly = 1 << 6,
		ConCmdFlags_Archive = 1 << 7,
		ConCmdFlags_Notify = 1 << 8,
		ConCmdFlags_UserInfo = 1 << 9,
		ConCmdFlags_PrintableOnly = 1 << 10,
		ConCmdFlags_Unlogged = 1 << 11,
		ConCmdFlags_NeverAsString = 1 << 12,
		ConCmdFlags_Replicated = 1 << 13,
		ConCmdFlags_Cheat = 1 << 14,
		ConCmdFlags_Demo = 1 << 16,
		ConCmdFlags_DontRecord = 1 << 17,
		ConCmdFlags_NotConnected = 1 << 22,
		ConCmdFlags_ArchieveXBox = 1 << 24,
		ConCmdFlags_ServerCanExecute = 1 << 28,
		ConCmdFlags_ServerCannotQuery = 1 << 29,
		ConCmdFlags_ClientCmdCanExecute = 1 << 30
	};
}
