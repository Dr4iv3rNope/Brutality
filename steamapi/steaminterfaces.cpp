#include "steaminterfaces.hpp"

#include "../util/debug/assert.hpp"

SteamInterfaces::SteamInterfaces()
{
	VALVE_SDK_INTERFACE_IMPL(client, "steamclient.dll", "SteamClient018");

	_pipeHandle = client->CreateSteamPipe();
	_userHandle = client->ConnectToGlobalUser(_pipeHandle);
	
	user = client->GetSteamUser(_userHandle, _pipeHandle, UTIL_CXOR("SteamUser020"));
	UTIL_ASSERT(user, "Failed to get Steam User interface");

	userstats = client->GetSteamUserStats(_userHandle, _pipeHandle, UTIL_CXOR("STEAMUSERSTATS_INTERFACE_VERSION011"));
	UTIL_ASSERT(userstats, "Failed to get Steam User Stats interface");
}

SteamInterfaces::~SteamInterfaces()
{
	client->ReleaseUser(_pipeHandle, _userHandle);
	client->ReleaseSteamPipe(_pipeHandle);
}
