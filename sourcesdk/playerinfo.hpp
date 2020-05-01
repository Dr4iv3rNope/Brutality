#pragma once
#include "const.hpp"

#include <cstdint>

namespace SourceSDK
{
	struct PlayerInfo final
	{
		char name[MAX_PLAYER_NAME_LENGTH];
		int userid;
		char steamid[SIGNED_GUID_LEN];
		std::uint32_t friendsID; // friends identification number
		char friendsName[MAX_PLAYER_NAME_LENGTH]; // friends name
		bool fakePlayer;
		bool isHLTV;
		CRC32 customFiles[MAX_CUSTOM_FILES]; // custom files CRC for this player
		std::uint8_t filesDownloaded;
	};
}
