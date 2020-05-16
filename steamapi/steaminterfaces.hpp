#pragma once
#include "../valvesdk/interfaces.hpp"

#include "steamclient.hpp"
#include "steamuser.hpp"
#include "steamuserstats.hpp"

class SteamInterfaces
{
private:
	SteamAPI::SteamPipeHandle _pipeHandle;
	SteamAPI::SteamUserHandle _userHandle;

public:
	SteamAPI::ISteamClient* client;
	SteamAPI::ISteamUser* user;
	SteamAPI::ISteamUserStats* userstats;

	SteamInterfaces();
	~SteamInterfaces();
};

extern SteamInterfaces* steamInterfaces;
