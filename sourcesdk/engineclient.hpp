#pragma once
#include "playerinfo.hpp"

namespace SourceSDK
{
	class AchievementMgr;

	class EngineClient final
	{
	public:
		void ServerCmd(const char* cmd, bool reliable = true);
		bool GetPlayerInfo(int entidx, PlayerInfo& info);
		void ClientCmdUnrestricted(const char* cmd);
		AchievementMgr* GetAchievementMgr() noexcept;
	};
}
