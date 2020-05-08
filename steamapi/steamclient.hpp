#pragma once
#include "../valvesdk/interfaces.hpp"

#include "steamuser.hpp"

namespace SteamAPI
{
	using SteamPipeHandle = int;

	class SteamClient
	{
	public:
		virtual SteamPipeHandle CreateSteamPipe() = 0;
		virtual bool ReleaseSteamPipe(SteamPipeHandle hSteamPipe) = 0;
		virtual SteamUserHandle ConnectToGlobalUser(SteamPipeHandle hSteamPipe) = 0;
	
	private:
		virtual void Unused0() = 0;

	public:
		virtual void ReleaseUser(SteamPipeHandle hSteamPipe, SteamUserHandle hUser) = 0;
		virtual SteamUser* GetSteamUser(SteamUserHandle hSteamUser, SteamPipeHandle hSteamPipe, const char* pchVersion) = 0;
	};

	VALVE_SDK_INTERFACE_DECL(SteamClient, client);
}
