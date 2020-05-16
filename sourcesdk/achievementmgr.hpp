#pragma once
#include <cstdint>

#include "../build.hpp"

#include "../util/pad.hpp"

namespace SourceSDK
{
	class AchievementMgr;
	struct Achievement final
	{
		UTIL_PAD(vmt, 12);

		const char* name;
		int achievementID;
		int flags; // ACH_* (not implemented)
		int goal;
		int progressMsgIncrement;
		int processMsgMinimum;
		int pointValue;
		bool hideUntilAchieved;

		struct
		{
			const char* inflictorClassname;
			const char* inflictorEntityName;
			const char* victimClassName;
			const char* attackerClassName;
			const char* mapName;
			const char* gameDir;
		} filter;

		const char** componentNames;
		int componentsCount;

		const char* componentPrefix;
		int componentPrefixLength;

		bool achieved;

		#if BUILD_GAME_IS_GMOD
		UTIL_PAD(0, 4);
		#endif

		int count;
		std::uint64_t componentBits;
		AchievementMgr* achievementMgr;
	};

	class AchievementMgr final
	{
	public:
		Achievement* GetAchievementByIndex(int idx);
		int GetAchievementCount();
		void AwardAchievement(int idx);
	};
}
