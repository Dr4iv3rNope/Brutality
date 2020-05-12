#pragma once
#include "../build.hpp"

#include "networkstringtable.hpp"
#include "playerinfo.hpp"
#include "vector.hpp"

namespace SourceSDK
{
	class NetChannel;

	enum class SignonState : int
	{
		None,
		Challange,
		Connected,
		New,
		PreSpawn,
		Spawn,
		Full,
		ChangeLevel
	};

	enum class NetTableId
	{
		ModelPreCache,
		GenericPreCache,
		SoundPreCache,
		DecalPreCache,
		InstanceBaseLine,
		LightStyle,
		UserInfo,
		ServerStartup,
		DownloadableFiles
	};

	class ClientState
	{
	#if BUILD_GAME_IS_GMOD
	private:
		char __pad0[12]; // vmt

	public:
		int socket;
		NetChannel* netChannel;

	private:
		char __pad1[16];

	public:
		char retryAddress[260];

	private:
		char __pad2[20];

	public:
		SignonState signonState;

	private:
		char __pad3[120];

	public:
		char currentMap[40];
		char shortCurrentMap[40];
		int maxClients;

	private:
		char __pad4[67792];

	public:
		Angle viewAngles;

	private:
		char __pad5[756];

	public:
		INetworkStringTable* networkStringTables[10];
	#endif

		inline std::string GetShortCurrentMap() const noexcept
		{
			return std::string(shortCurrentMap, sizeof(shortCurrentMap));
		}

		inline std::string GetCurrentMap() const noexcept
		{
			return std::string(currentMap, sizeof(currentMap));
		}

		inline bool IsInGame() const noexcept
		{
			return signonState == SignonState::Full;
		}

		inline bool IsConnected() const noexcept
		{
			return signonState > SignonState::Connected;
		}

		inline std::string GetRetryAddress() const noexcept
		{
			return std::string(retryAddress, sizeof(retryAddress));
		}
	};	
}
