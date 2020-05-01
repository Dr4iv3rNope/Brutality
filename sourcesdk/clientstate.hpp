#pragma once
#include "sdk.hpp"
#include "interfaces.hpp"
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
	#if SOURCE_SDK_IS_GMOD
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
		INetworkStringTable* const networkStringTables[10];
	#endif
	};

	SOURCE_SDK_INTERFACE_DECL(ClientState, clientState);

	inline auto IsInGame() noexcept
	{
		return clientState->signonState == SignonState::Full;
	}
}
