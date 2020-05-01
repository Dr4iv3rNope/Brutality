#pragma once
#include "interfaces.hpp"
#include "playerinfo.hpp"

namespace SourceSDK
{
	class EngineClient final
	{
	public:
		//[[deprecated("use ImGui::GetIO().DisplaySize")]]
		//void GetScreenSize(int& w, int& h);
		
		void ServerCmd(const char* cmd);
		bool GetPlayerInfo(int entidx, PlayerInfo& info);
		void ClientCmdUnrestricted(const char* cmd);
	};

	SOURCE_SDK_INTERFACE_DECL(EngineClient, engine);
}
