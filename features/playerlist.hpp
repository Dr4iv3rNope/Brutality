#pragma once
#include <string>

namespace Features
{
	namespace PlayerList
	{
		enum class PlayerType
		{
			Normal,
			Dangerous,	// used when we want leave this player alone
			Friend,		// your friend, nothing more
			Rage		// other cheater or player you dont like
		};

		//
		// for external usage
		//

		extern bool GetPlayerType(int idx, PlayerType& type);

		void LoadCache();
		void SaveCache();

		// used in FrameStageNotify
		void Update();

		void RegisterWindow() noexcept;
	}
}
