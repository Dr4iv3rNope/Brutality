#pragma once
#include <cstdint>

namespace SteamAPI
{
	class ISteamUserStats
	{
	private:
		virtual void RequestCurrentStats() = 0;
		virtual void GetStatInt32() = 0;
		virtual void GetStatFloat() = 0;
		virtual void SetStatInt32() = 0;
		virtual void SetStatFloat() = 0;
		virtual void UpdateAvgRateStat() = 0;

	public:
		virtual bool GetAchievement(const char* name, bool* achieved) = 0;
		virtual bool SetAchievement(const char* name) = 0;
		virtual bool ClearAchievement(const char* name) = 0;

		virtual bool GetAchievementAndUnlockTime(
			const char* name,
			bool* achieved,
			std::uint32_t* unlock_time
		) = 0;

		virtual bool StoreStats() = 0;

	private:
		virtual void GetAchievementIcon() = 0;
		virtual void GetAchievementDisplayAttribute() = 0;
		virtual void IndicateAchievementProgress() = 0;

	public:
		virtual std::uint32_t GetAchievementCount() = 0;
		virtual const char* GetAchievementName(std::uint32_t achievement) = 0;
	};
}
