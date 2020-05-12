#include "steaminterfaces.hpp"

SteamInterfaces::SteamInterfaces()
{
	VALVE_SDK_INTERFACE_IMPL(client, "steamclient.dll", "SteamClient018");

	_pipeHandle = client->CreateSteamPipe();
	_userHandle = client->ConnectToGlobalUser(_pipeHandle);
	
	user = client->GetSteamUser(_userHandle, _pipeHandle, UTIL_CXOR("SteamUser020"));
}

SteamInterfaces::~SteamInterfaces()
{
	client->ReleaseUser(_pipeHandle, _userHandle);
	user = nullptr;

	client->ReleaseSteamPipe(_pipeHandle);
}
