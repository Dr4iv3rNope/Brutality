#pragma once
#include "interfaces.hpp"
#include "playerinfo.hpp"

namespace SourceSDK
{
	class EngineClient final
	{
	public:
		void ServerCmd(const char* cmd, bool reliable = true);
		bool GetPlayerInfo(int entidx, PlayerInfo& info);
		void ClientCmdUnrestricted(const char* cmd);
	};

	SOURCE_SDK_INTERFACE_DECL(EngineClient, engine);
}
