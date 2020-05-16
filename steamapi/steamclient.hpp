#pragma once
#include "steamuser.hpp"

namespace SteamAPI
{
	using SteamPipeHandle = int;
	
	class ISteamUserStats;
	class ISteamClient
	{
	public:
		virtual SteamPipeHandle CreateSteamPipe() = 0;
		virtual bool ReleaseSteamPipe(SteamPipeHandle hSteamPipe) = 0;
		virtual SteamUserHandle ConnectToGlobalUser(SteamPipeHandle hSteamPipe) = 0;
	
	private:
		virtual void Unused0() = 0;

	public:
		virtual void ReleaseUser(SteamPipeHandle hSteamPipe, SteamUserHandle hUser) = 0;

		virtual ISteamUser* GetSteamUser(
			SteamUserHandle hSteamUser,
			SteamPipeHandle hSteamPipe,
			const char* pchVersion
		) = 0;

	private:
		virtual void GetSteamGameServer() = 0;
		virtual void GetLocalIPBinding() = 0;
		virtual void GetSteamFriends() = 0;
		virtual void GetSteamUtils() = 0;
		virtual void GetSteamMatchmakingServers() = 0;
		virtual void GetSteamGenericInterface() = 0;
		
	public:
		virtual ISteamUserStats* GetSteamUserStats(
			SteamUserHandle hSteamUser,
			SteamPipeHandle hSteamPipe,
			const char* pchVersion
		) = 0;
	};
}
