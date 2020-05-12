#pragma once
#include "../valvesdk/interfaces.hpp"

#include "steamclient.hpp"
#include "steamuser.hpp"

class SteamInterfaces
{
private:
	SteamAPI::SteamPipeHandle _pipeHandle;
	SteamAPI::SteamUserHandle _userHandle;

public:
	SteamAPI::SteamClient* client;
	SteamAPI::SteamUser* user;

	SteamInterfaces();
	~SteamInterfaces();
};

extern SteamInterfaces* steamInterfaces;
