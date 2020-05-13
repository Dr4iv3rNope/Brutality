#include "achievementmgr.hpp"

#include "../util/vmt.hpp"
#include "../util/pattern.hpp"

SourceSDK::Achievement* SourceSDK::AchievementMgr::GetAchievementByIndex(int idx)
{
	// "achievement_earned" in ClientModeShared::FireGameEvent
	#if BUILD_GAME_IS_GMOD
	static const auto offset
	{
		(*(std::uint8_t*)UTIL_XFIND_PATTERN(
			"client.dll",
			"8B 10 8B C8 53 FF 52 ?? 85 C0 0F 84",
			7
		)) / 4
	};
	#endif

	return Util::Vmt::CallMethod<Achievement*, int>(this, offset, idx);
}

int SourceSDK::AchievementMgr::GetAchievementCount()
{
	#if BUILD_GAME_IS_GMOD
	// "achievement_pack_combo" "HideAchieved" in 
	// CAchievementsDialog::CAchievementsDialog
	static const auto offset
	{
		(*(std::uint8_t*)UTIL_XFIND_PATTERN(
			"gameui.dll",
			"FF 87 ?? ?? ?? ?? 8B 0D ?? ?? ?? ?? 85 C9 0F 84 ?? ?? ?? ?? 8B 01 8B 40",
			24
		)) / 4
	};
	#endif

	return Util::Vmt::CallMethod<int>(this, offset);
}
